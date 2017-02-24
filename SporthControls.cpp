/*
 * SporthControls.cpp - controls for Sporth
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


#include <iostream>
#include <QDomElement>

#include "SporthControls.h"
#include "Sporth.h"
#include "Engine.h"
#include "Song.h"

SporthControls::SporthControls( SporthEffect* effect ) :
	EffectControls( effect ),
	m_effect( effect ),
	m_inputGainModel( 0.0f, 0, 1, 0.001f, this, tr( "P0" ) ),
	m_sizeModel( 0.0f, 0.0f, 1.0f, 0.001f, this, tr( "P1" ) ),
	m_colorModel( 0.0f, 0.0f, 1.0f, 0.001f, this, tr( "P2" ) ),
	m_outputGainModel( 0.0f, 0.0f, 1.0f, 0.001f, this, tr( "P3" ) ),
	m_compileModel( 0.0f, 0.0f, 1.0f, 0.5f, this, tr( "Compile" ) )
{
}

void SporthControls::changeControl()
{
}

void SporthControls::loadSettings( const QDomElement& _this )
{
	m_inputGainModel.loadSettings( _this, "input_gain" );
	m_sizeModel.loadSettings( _this, "size" );
	m_colorModel.loadSettings( _this, "color" );
	m_outputGainModel.loadSettings( _this, "output_gain" );
	m_compileModel.loadSettings( _this, "compile" );
    sporth_string = _this.attribute("sporth");
}

void SporthControls::saveSettings( QDomDocument& doc, QDomElement& _this )
{
	m_inputGainModel.saveSettings( doc, _this, "input_gain" ); 
	m_sizeModel.saveSettings( doc, _this, "size" ); 
	m_colorModel.saveSettings( doc, _this, "color" );
	m_outputGainModel.saveSettings( doc, _this, "output_gain" ); 
	m_compileModel.saveSettings( doc, _this, "compile" ); 
    _this.setAttribute("sporth", textEditor->toPlainText());
}
