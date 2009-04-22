/******************************************************************************************
 *
 * HighNoon - Duell im All
 * Copyright (c) 2005 Patrick Gerdsmeier <patrick@gerdsmeier.net>
 *
 * "language.hpp"
 * 
 * This class contains all Screentexts as methods.
 * Texts depend on their active language. You can 
 * toggle the language via "toogle()".
 * Actually HighMoon has English and German Mode.
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

#ifndef __LANGUAGE_HPP__
#define __LANGUAGE_HPP__

#include <iostream>

enum Languagetype { ENGLISH=0, GERMAN=1 };

class Language
{
public:
	Language() : language(ENGLISH) {};
	
	void toggle();
	
	char* getWindowtext();
	
	std::string getTitletext( int id );

	std::string getScrollertext();
	
	std::string getPlayertext( int id );
	
	std::string getComputertext( int strength );
	
	std::string getShieldtext(  int energy );
	
	std::string getWinnertext( int winner );
	
	std::string getWarptext( int seed, int planets );

	std::string getWarptext();
	
private:
	Languagetype language;

};

#endif

