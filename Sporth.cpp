/*
 * Sporth.cpp - A native reverb based on an algorithm by Sean Costello
 *
 * This file is part of LMMS - http://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include <math.h>
#include <iostream>

#include "Sporth.h"

#include "Song.h"
#include "embed.cpp"

#define DB2LIN(X) pow(10, X / 20.0f);

extern "C"
{

static int sporth_lmms_in(sporth_stack *stack, void *ud);
Plugin::Descriptor PLUGIN_EXPORT sporthedit_plugin_descriptor =
{
	STRINGIFY( PLUGIN_NAME ),
	"Sporth",
	QT_TRANSLATE_NOOP( "pluginBrowser", "Sporth Editor." ),
	"Paul Batchelor",
	0x0124,
	Plugin::Effect,
	new PluginPixmapLoader( "logo" ),
	NULL,
	NULL
} ;

}

SporthEffect::SporthEffect( Model* parent, const Descriptor::SubPluginFeatures::Key* key ) :
	Effect( &sporthedit_plugin_descriptor, parent, key ),
	m_sporthControls( this )
{
	sp_create(&sp);
	sp->sr = Engine::mixer()->processingSampleRate();
    plumber_register(&pd);
    pd.sporth.flist[SPORTH_IN - SPORTH_FOFFSET].func = sporth_lmms_in;
    plumber_init(&pd);
    pd.sp = sp;
    pd.ud = &inL;
    plumber_parse_string(&pd, "0 0");
    plumber_compute(&pd, PLUMBER_INIT);
    prev = -1;
    sporth_compile = 0;
    please_compile = 0;
    button_prev = 0;
}

SporthEffect::~SporthEffect()
{
    plumber_clean(&pd);
	sp_destroy(&sp);
}

bool SporthEffect::processAudioBuffer( sampleFrame* buf, const fpp_t frames )
{
	if( !isEnabled() || !isRunning () )
	{
		return( false );
	}

	double outSum = 0.0;

    /* until LMMS gets their shit together, wet/dry will be calculated the
     * following way:
     *
     * In LMMS, the dry value goes from range 0-2, while the wet value is -1,1
     * Ideally, this range should be 0-1, so the wet signal is taken and scaled
     * for both the dry and wet signals.
     */

	const float d = (1 + wetLevel()) * 0.5;
	const float w = 1 - d;

	SPFLOAT outL, outR;
	
	ValueBuffer * p0Buf = m_sporthControls.m_P0Model.valueBuffer();
	ValueBuffer * p1Buf = m_sporthControls.m_P1Model.valueBuffer();
	ValueBuffer * p2Buf = m_sporthControls.m_P2Model.valueBuffer();
	ValueBuffer * p3Buf = m_sporthControls.m_P3Model.valueBuffer();
	ValueBuffer * compileBuf = m_sporthControls.m_compileModel.valueBuffer();

		
    SPFLOAT compile = m_sporthControls.m_compileModel.value();
    
    SPFLOAT button_compile = m_sporthControls.m_compileButtonModel.value();

    if((compile != prev && prev != -1 && compile > 0) || 
    sporth_compile || 
    (button_compile == 1.0 && button_prev != 1.0)) {
        prev = compile;
        recompile();
    }

    button_prev = button_compile;

	for( fpp_t f = 0; f < frames; ++f )
	{

        prev = compile;

        inL = buf[f][0];
        inR = buf[f][1];

        pd.p[0] = (SPFLOAT)(p0Buf ? 
			p0Buf ->values()[f] 
			: m_sporthControls.m_P0Model.value());

        pd.p[1] = (SPFLOAT)(p1Buf ? 
			p1Buf ->values()[f] 
			: m_sporthControls.m_P1Model.value());
        
        pd.p[2] = (SPFLOAT)(p2Buf ? 
			p2Buf ->values()[f] 
			: m_sporthControls.m_P2Model.value());
       
        pd.p[3] = (SPFLOAT)(p3Buf ? 
			p3Buf ->values()[f] 
			: m_sporthControls.m_P3Model.value());

        plumber_compute(&pd, PLUMBER_COMPUTE);
        outR = sporth_stack_pop_float(&pd.sporth.stack);
        outL = sporth_stack_pop_float(&pd.sporth.stack);

		buf[f][0] = d * buf[f][0] + w * outL;
		buf[f][1] = d * buf[f][1] + w * outR;

		outSum += 1 + buf[f][0]*buf[f][0] + buf[f][1]*buf[f][1];

        if(sporth_compile != 0 && please_compile == 0) {
            please_compile = -1;
            recompile();
        } else if(please_compile < 0) please_compile = 0;

	}


	checkGate( outSum / frames );

	return isRunning();
}
    
void SporthEffect::recompile()
{
    int error;
    m_sporthControls.sporth_string = m_sporthControls.textEditor->toPlainText();
    std::string txt = m_sporthControls.sporth_string.toUtf8().constData();
    std::cout << txt << "\n";
    inL = 0;
    inR = 0;
    /* TODO: make worth with automation */
    bpm = Engine::getSong()->getTempo();

    /* file pointer needs to be NULL for reinit to work with strings */
    pd.fp = NULL;
    plumber_reinit(&pd);
    plumber_ftmap_delete(&pd, 0);
    plumber_ftmap_add_userdata(&pd, "inL", &inL);
    plumber_ftmap_add_userdata(&pd, "inR", &inR);
    plumber_ftmap_add_userdata(&pd, "bpm", &bpm);
    plumber_ftmap_add_userdata(&pd, "compile", &sporth_compile);
    plumber_ftmap_delete(&pd, 1);
    error = plumber_reparse_string(&pd, (char *)txt.c_str());
    plumber_swap(&pd, error);
}

extern "C"
{

// necessary for getting instance out of shared lib
Plugin * PLUGIN_EXPORT lmms_plugin_main( Model* parent, void* data )
{
	return new SporthEffect( 
		parent, 
		static_cast<const Plugin::Descriptor::SubPluginFeatures::Key*>(data) 
	);
}

static int sporth_lmms_in(sporth_stack *stack, void *ud)
{
    plumber_data *pd = (plumber_data *) ud;

    SPFLOAT *in = (SPFLOAT *)pd->ud;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: creating\n");
#endif
            plumber_add_ugen(pd, SPORTH_IN, NULL);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: initialising.\n");
#endif

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            sporth_stack_push_float(stack, *in);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: destroying.\n");
#endif

            break;

        default:
            fprintf(stderr, "Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

}
