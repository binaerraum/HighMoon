/******************************************************************************************
 *
 * HighNoon - Duell im All Version 1.0
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "language.cpp"
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
#include "constants.hpp"

const int MAXLANGUAGES=5;

Language::Language()
:
	language(ENGLISH)
{
	verbose( "Initializing Language" ); 
}

void Language::toggle()
{
	language=Languagetype((language+1)%MAXLANGUAGES);
}
	
char* Language::getWindowtext() const
{
	char* r[]={
		"HighMoon - Duel in Space by Patrick Gerdsmeier",
		"HighMoon - Duell im All von Patrick Gerdsmeier",
		"HighMoon - Duel dans l'Univers de Patrick Gerdsmeier",
		"HighMoon - Przeciwnicy w Kosmosie od Patrick Gerdsmeier",
		"HighMoon - Duelo uo Universo de Patrick Gerdsmeier"
	};

	return r[language];
}

std::string Language::getTitletext( int id ) const
{	
	const std::string r[][_TITLETEXT]={
		{
			"HIGHMOON",
			"",
			"DUEL IN SPACE V"+VERSION,
			"FEBRUARY 2005",
			"BY PATRICK GERDSMEIER,",
			"PATRICK@GERDSMEIER.NET",
			"LICENSED UNDER THE GPL",
			"",
			"F1 FOR HELP",
			"F2 DEUTSCH"
		},

		{
			"HIGHMOON",
			"",
			"DUELL IM ALL V"+VERSION,
			"FEBRUAR 2005",
			"VON PATRICK GERDSMEIER,",
			"PATRICK@GERDSMEIER.NET",
			"LIZENSIERT UNTER DER GPL",
			"",
			"F1 FUER HILFE",
			"F2 FRANgAIS"	
		},

		{
			"HIGHMOON",
			"",
			"DUEL DANS L'UNIVERS V"+VERSION,
			"FiVRIER 2005",
			"DE PATRICK GERDSMEIER,",
			"PATRICK@GERDSMEIER.NET",
			"LICENCE SOUS LE GPL",
			"",
			"F1 POUR L'AIDE",
			"F2 POLSKI"
		},

		{
			"HIGHMOON",
			"",
			"PRZECIWNICY W KOSMOSIE V"+VERSION,
			"LUTY 2005",
			"OD PATRICK GERDSMEIER,",
			"PATRICK@GERDSMEIER.NET",
			"POD LICENCJa GPL",
			"",
			"F1 POMOC",
			"F2 PORTUGUESE"
		},

		{
			"HIGHMOON",
			"",
			"DUELO UO UNIVERSO V"+VERSION,
			"FEVEREIRO 2005",
			"DE PATRICK GERDSMEIER,",
			"PATRICK@GERDSMEIER.NET",
			"LICENgA A BAIXO DE GPL",
			"",
			"F1 PARA AJUDA",
			"F2 ENGLISH"
		}
	
	};
	
	return r[language][id];
}

std::string Language::getScrollertext() const
{	
	const std::string r[]={
		"THE KEYS 1, 2 AND 3 FOR MODE OF GAME - "
		"TAB CHANGES THE GALAXY - "
		"CURSOR LEFT AND RIGHT TO SET THE SHOOTANGLE - "
		"CURSOR UP AND DOWN TO MOVE THE SPACESHIP - "
		"KEEP SPACE PRESSED TO INCREASE SHOOTPOWER - RELEASE SPACE TO FIRE - "
		"F TOGGLES WINDOW AND FULLSCREEN MODE. "
		"C TOGGLES COMPUTERSTRENGTH 1..5.   "
		"HIGHMOON THE DUEL IN SPACE WAS WRITTEN IN 2005 BY PATRICK GERDSMEIER AND IS LICENSED UNDER THE GPL.   "
		"HAVE FUN!   DEDICATED TO ULI     ",

		"DIE TASTEN 1, 2 UND 3 FUER SPIELMODUS - "
		"TAB WECHSELT DIE GALAXIE - "
		"CURSOR LINKS UND RECHTS STELLT DEN SCHUSSWINKEL EIN - "
		"CURSOR OBEN UND UNTEN BEWEGEN DAS UFO - "
		"LEERTASTE GEDRUECKT LASSEN ZUM ERHOEHEN DER SCHUSSSTAERKE - LEERTASTE LOSLASSEN ZUM FEUERN - "
		"F WECHSELT ZWISCHEN FENSTER UND FULLSCREEN MODUS. "
		"C WECHSELT DIE COMPUTERSTAERKE 1..5.   "
		"HIGHMOON DAS DUELL IM ALL WURDE GESCHRIEBEN 2005 VON PATRICK GERDSMEIER UND IST LIZENSIERT UNTER DER GPL.   VIEL SPASS!   "
		"GEWIDMET MEINER ULI     ",

		"LES CLiS 1, 2 ET 3 POUR MODE DU JEU - "
		"LE TABULATEUR CHANGE LA GALAXIE - "
		"CURSEUR h GAUCHE ET DROITE PLACES L'ANGLE DU TIR - "
		"CURSEUR EN HAUT ET VERS LE BAS DiPLACER L'UFO - "
		"LA CLi LAISSER AUGMENTE PiNIBLE LE POUVOIR DU TIR - "
		"CLi LIBEiRER VISANT ALIMENTER - "
		"LE F CHANGE FENiTRE-IMAGE PLEINE. "
		"LE C CHANGE LE POUVOIR DE L'ORDINATEUR 1..5.   "
		"HIGHMOON LE DUEL DANS L'UNIVERS 2005 DE PATRICK GERDSMEIER 2005. LICENCE SOUS LE GPL.   "
		"BEAUCOUP D'AMUSEMENT!    POUR MA ULI     ",
		
		"WYBdR RODZAJU GRY 1, 2, 3 - "
		"TABULATOR ZMIENIA GALAKTYKb - "
		"KURSOR LEWY IPRAWY DO USTAWIENIA KaTA STRZEcU - "
		"KURSOR GdRA, Ddc PORUSZA UFO - "
		"PRZYTRZYMANIE SPACJI OKREeLA SIcb STRZAcU - "
		"F ZMIEWIA ROZMIAR OKNA. "
		"C ZMIAWA POZIOMUTRUDNOSCI 1..5.   "
		"HIGHMOON PRZECIWNICY W KOSMOSIE. NAPISANE PRZEZ PATRICK GERDSMEIER LUTY 2005. POD LICENCJa GPL. DUfo PRZYJEMNOeCI!   ",

		"AS TECLAS 1, 2, 3 PARA OS MODOS DA JOGO - "
		"TABULATOR TROCA DE GALAXIA - "
		"CURSOR ESQUERDO E DIREITO PARA O ANGULO DE ALVO - "
		"CURSOR BAIXO E ALTO MOVE O UFO - "
		"ESPAgO DEIXO POLUSADO PARA FAZER A ALVO MAIS FORTE - "
		"DEIXA ESPAQO PARA ATIRAR - "
		"F TROCA ENTRE JANELA E MODO DE ABERTURA. "
		"C TROCA A FORgA DO COMPUTATOR 1..5.   "
		"HIGHMOON O DUELO UO UNIVERSO ESTAVA ESCRIVENDO 2005 DE PATRICK GERDSMEIER 2005. LICENga A BAIXO DE GPL.   "
		"BOA DIVERSAOhO!   DEDICATdRIA PARA MINHA ULI     "
	};

	return r[language];
}

std::string Language::getPlayertext( int id ) const 
{
	const std::string p[]={
		"PLAYER",
		"SPIELER",
		"JOUEUR",
		"GRACZ",
		"JOGADOR"
	};	

	std::ostringstream r;
	
	r << p[language] << " " << id;

	return r.str();
}

std::string Language::getComputertext( int strength ) const
{
	const std::string p[][5]={
		{ "NOVICE", "RECRUIT", "SOLDIER", "OFFICER", "GENERAL" },
		{ "NEULING", "REKRUT", "SOLDAT", "OFFIZIER", "GENERAL" },
		{ "DiBUTANT", "REKRUT", "SOLDAT", "AGENT", "GiNiRALE" },
		{ "NOWICJUSZ", "REKRUT", "fOcNIERZ", "OFICER", "GENERAc" },
		{ "INICIANTE", "REKRUTANTE", "SOLDADO", "OFICIAL", "GENERAL" }
	};

	std::ostringstream r;

	if (strength>4) 
		strength=4;

	r << p[language][strength];
	
	return r.str();
}

std::string Language::getShieldtext( int energy ) const 
{
	const std::string p[]={
		"SHIELD",
		"SCHILD",
		"BOUCLIER",
		"TARCZA",
		"ARMADURA"
	};

	std::ostringstream r;
	
	r << p[language] << " " << energy;

	return r.str();
}

std::string Language::getWinnertext( int winner ) const 
{
	const std::string p[][3]={
		{
			"THE COMPUTER WON!",
			"PLAYER", "WON!"
		},
		{
			"DER COMPUTER HAT GEWONNEN!",
			"SPIELER", "HAT GEWONNEN!"
		},
		{
			"L'ORDNINATEUR A GAGNi!",
			"LE JOUEUR",
			"A GAGNi!"
		},
		{
			"KOMPUTER WYGRAc!",
			"GRACZ", "WYGRAc!"
		},
		{
			"O COMPUTADOR GANHOU!",
			"O JOGADOR", "GANHOU!"
		}

	};

	std::ostringstream r;
	
	if (winner==-1)
		r << p[language][0];
	else
		r << p[language][1] << " " << winner << " "  << p[language][2];

	return r.str();
}

std::string Language::getWarptext( int seed, int planets ) const
{
	const std::string p[]={
		"NUMBER",
		"NUMMER",
		"NUMiRO",
		"GALAKTYKI",
		"NjMERO"
	};

	std::ostringstream r;

	r << p[language] << " " << seed << "." << planets;

	return r.str();
}

std::string Language::getWarptext() const
{
	const std::string r[]={
		"WARPING TO GALAXY",
		"WARPE ZUR GALAXIE",
		"WARPE h LA GALAXIE",
		"TELEPORTACJA DO",
		"WARPE PELA GALhXIA"
	};

	return r[language];
}


