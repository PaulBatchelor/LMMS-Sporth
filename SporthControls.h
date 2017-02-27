/*
 * SporthControls.h 
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

#ifndef SPORTH_CONTROLS_H
#define SPORTH_CONTROLS_H

#include <QPlainTextEdit>
#include "EffectControls.h"
#include "SporthControlDialog.h"
#include "Knob.h"
#include "PixmapButton.h"
#include "LedCheckbox.h"


class SporthEffect;

class SporthControls : public EffectControls
{
	Q_OBJECT
public:
	SporthControls( SporthEffect* effect );
	virtual ~SporthControls()
	{
	}

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );
	inline virtual QString nodeName() const
	{
		return "SporthControls";
	}

	virtual int controlCount()
	{
		return 4;
	}

	virtual EffectControlDialog* createView()
	{
		return new SporthControlDialog( this );
	}



private slots:
	void changeControl();
	void updateLayout();

private:
	SporthEffect* m_effect;
	FloatModel m_P0Model;
	FloatModel m_P1Model;
	FloatModel m_P2Model;
	FloatModel m_P3Model;
    FloatModel m_compileModel;
    BoolModel m_compileButtonModel;

    QPlainTextEdit *textEditor;
    QString sporth_string;

	friend class SporthControlDialog;
	friend class SporthEffect;

} ;

#endif
