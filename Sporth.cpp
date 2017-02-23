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
#include "Sporth.h"

#include "embed.cpp"

#define DB2LIN(X) pow(10, X / 20.0f);

extern "C"
{

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
	m_reverbSCControls( this )
{
	sp_create(&sp);
	sp->sr = Engine::mixer()->processingSampleRate();
    plumber_register(&pd);
    plumber_init(&pd);
    pd.sp = sp;
    plumber_parse_string(&pd, "0 p 'val' print 100 300 scale 0.1 sine dup");
    plumber_compute(&pd, PLUMBER_INIT);

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
	const float d = (dryLevel() + 1) * 0.5;
	const float w = (wetLevel() + 1) * 0.5;

	SPFLOAT outL, outR;
	
	ValueBuffer * inGainBuf = m_reverbSCControls.m_inputGainModel.valueBuffer();
	ValueBuffer * sizeBuf = m_reverbSCControls.m_sizeModel.valueBuffer();
	ValueBuffer * colorBuf = m_reverbSCControls.m_colorModel.valueBuffer();
	ValueBuffer * outGainBuf = m_reverbSCControls.m_outputGainModel.valueBuffer();

	for( fpp_t f = 0; f < frames; ++f )
	{
	
		const SPFLOAT inGain = (SPFLOAT)DB2LIN((inGainBuf ? 
			inGainBuf->values()[f] 
			: m_reverbSCControls.m_inputGainModel.value()));
		const SPFLOAT outGain = (SPFLOAT)DB2LIN((outGainBuf ? 
			outGainBuf->values()[f] 
			: m_reverbSCControls.m_outputGainModel.value()));

        pd.p[0] = inGain;
        plumber_compute(&pd, PLUMBER_COMPUTE);
        outR = sporth_stack_pop_float(&pd.sporth.stack);
        outL = sporth_stack_pop_float(&pd.sporth.stack);

		buf[f][0] = d * buf[f][0] + w * outL * outGain;
		buf[f][1] = d * buf[f][1] + w * outR * outGain;
		outSum += buf[f][0]*buf[f][0] + buf[f][1]*buf[f][1];
	}


	checkGate( outSum / frames );

	return isRunning();
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

}
