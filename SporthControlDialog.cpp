/*
 * SporthControlDialog.cpp - control dialog for Sporth
 *
 * Copyright (c) 2017 Paul Batchelor
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

#include <QLayout>
#include <iostream>

#include "SporthControlDialog.h"
#include "SporthControls.h"
#include "embed.h"

SporthControlDialog::SporthControlDialog( SporthControls* controls ) :
	EffectControlDialog( controls )
{
	setAutoFillBackground( true );
	QPalette pal;
	pal.setBrush( backgroundRole(), PLUGIN_NAME::getIconPixmap( "artwork" ) );
	setPalette( pal );
	setFixedSize( 332, 500 );
	
	Knob * inputGainKnob = new Knob( knobBright_26, this);
	inputGainKnob -> move( 16, 10 );
	inputGainKnob->setModel( &controls->m_P0Model);
	inputGainKnob->setLabel( tr( "P0" ) );
	inputGainKnob->setHintText( tr( "P0:" ) , "" );

	Knob * sizeKnob = new Knob( knobBright_26, this);
	sizeKnob -> move( 57, 10 );
	sizeKnob->setModel( &controls->m_P1Model);
	sizeKnob->setLabel( tr( "P1" ) );
	sizeKnob->setHintText( tr( "P1:" ) , "" );

	Knob * colorKnob = new Knob( knobBright_26, this);
	colorKnob -> move( 98, 10 );
	colorKnob->setModel( &controls->m_P2Model);
	colorKnob->setLabel( tr( "P2" ) );
	colorKnob->setHintText( tr( "P2:" ) , "" );
	
    Knob * P3Knob = new Knob( knobBright_26, this);
	P3Knob-> move( 139, 10 );
	P3Knob->setModel( &controls->m_P3Model);
	P3Knob->setLabel( tr( "P3" ) );
	P3Knob->setHintText( tr( "P3:" ) , "" );

    Knob *but  = new Knob(knobBright_26, this);
    but->move( (300 - 41), 10 );
	but->setLabel( tr( "Compile" ) );
    but->setModel(&controls->m_compileModel);
	but->setHintText( tr( "Compile" ) , "" );
    
    LedCheckBox *box = new LedCheckBox("", this);
    box->move( (300 - 2 * 41), 10 );
    box->setModel(&controls->m_compileButtonModel);

    controls->textEditor = new QPlainTextEdit(this);
    controls->textEditor->move(16, 50);
    controls->textEditor->resize(300, 400);
    
    QFont font = controls->textEditor->document()->defaultFont();
    font.setFamily("Courier New");
    controls->textEditor->document()->setDefaultFont(font);
    controls->textEditor->document()->setPlainText(controls->sporth_string);
}
