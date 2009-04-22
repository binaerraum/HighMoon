/******************************************************************************************
 *
 * HighNoon - Duell im All Version 1.0
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "language.hpp"
 * 
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ******************************************************************************************/

#include <sstream>

#include "language.hpp"

void Language::toggle()
{
	language++;
	language=language%2;
}
	
char* Language::getWindowtext()
{
	if (language==1) return "HighMoon - Duel in Space by Patrick Gerdsmeier, patrick@gerdsmeier.net";

	return "HighMoon - Duell im All von Patrick Gerdsmeier, patrick@gerdsmeier.net";
}

std::string Language::getTitletext( int id )
{	
	const std::string TITLETEXT[]={
		"HIGHMOON",
		"",
		"DUELL IM ALL",
		"VON PATRICK GERDSMEIER",
		"JANUAR 2005",
		"LIZENSIERT UNTER DER GPL",
		"",
		"F1 FUER HILFE",
		"F2 FOR ENGLISH"	
	};		
	const std::string TITLETEXT_e[]={
		"HIGHMOON",
		"",
		"DUEL IN SPACE",
		"BY PATRICK GERDSMEIER",
		"JANUARY 2005",
		"LICENSED UNDER THE GPL",
		"",
		"F1 FOR HELP",
		"F2 FUER DEUTSCH"
	};

	if (language==1) return TITLETEXT_e[id];
	
	return TITLETEXT[id];
}

std::string Language::getScrollertext()
{	
	const std::string SCROLLERTEXT="DIE TASTEN  1  2  3  FUER SPIELMODUS   "
		"TAB WECHLSELT DIE GALAXIE   "
		"CURSOR LINKS UND RECHTS STELLT DEN SCHUSSWINKEL EIN   "
		"CURSOR OBEN UND UNTEN BEWEGEN DAS UFO   "
		"LEERTASTE GEDRUECKT LASSEN ZUM ERHOEHEN DER SCHUSSSTAERKE   LEERTASTE LOSLASSEN ZUM FEUERN   "
		"F WECHSELT ZWISCHEN FENSTER UND FULLSCREEN MODUS   "
		"HIGHMOON DAS DUELL IM ALL WURDE GESCHRIEBEN IM JANUAR 2005 VON PATRICK GERDSMEIER UND IST LIZENSIERT UNTER DER GPL   VIEL SPASS   "
		"GEWIDMET MEINER ULI     ";
	const std::string SCROLLERTEXT_e="THE KEYS  1  2  3  FOR GAMEMODE   "
		"TAB CHANGES THE GALAXY   "
		"CURSOR LEFT UND RIGHT TO SET THE SHOOTANGLE   "
		"CURSOR UP AND DOWN TO MOVE THE SPACESHIP   "
		"KEEP SPACE PRESSED TO INCREASE SHOOTPOWER   RELEASE SPACE TO FIRE   "
		"F TOGGLES WINDOW AND FULLSCREEN MODE   "
		"HIGHMOON THE DUEL IN SPACE WAS WRITTEN IN JANUARY 2005 BY PATRICK GERDSMEIER AND IS LICENSED UNDER THE GPL   "
		"HAVE FUN   DEDICATED TO ULI     ";

	if (language==1) return SCROLLERTEXT_e;
	
	return SCROLLERTEXT;
}

std::string Language::getPlayertext( int id ) 
{
	std::ostringstream os;
	
	if (language==1) {
		os << "PLAYER " << id;

		return os.str();
	}

	os << "SPIELER " << id;

	return os.str();
}

std::string Language::getComputertext() 
{
	return "COMPUTER";
}

std::string Language::getShieldtext( int energy ) 
{
	std::ostringstream os;
	
	if (language==1) {
		os << "SHIELD " << energy;

		return os.str();
	}

	os << "SCHILD " << energy;

	return os.str();
}

std::string Language::getWinnertext( int winner ) 
{
	std::ostringstream os;
	
	if (language==1) {
		if (winner==-1) return "THE COMPUTER WON";
		else os << "PLAYER " << winner << " WON";

		return os.str();
	}

	if (winner==-1) return "DER COMPUTER HAT GEWONNEN";
	else os << "SPIELER " << winner << " HAT GEWONNEN";

	return os.str();
}

std::string Language::getWarptext( int seed, int planets )
{
	std::ostringstream os;

	if (language==1) {
		os << "NUMBER " << seed << " " << planets;

		return os.str();
	}

	os << "NUMMER " << seed << " " << planets;

	return os.str();
}

std::string Language::getWarptext()
{
	if (language==1) return "WARPING TO GALAXY";
	
	return "WARPE ZUR GALAXY";
}


