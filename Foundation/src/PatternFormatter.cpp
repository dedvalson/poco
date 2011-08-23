//
// PatternFormatter.cpp
//
// $Id: //poco/1.4/Foundation/src/PatternFormatter.cpp#2 $
//
// Library: Foundation
// Package: Logging
// Module:  PatternFormatter
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/PatternFormatter.h"
#include "Poco/Message.h"
#include "Poco/NumberFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTime.h"
#include "Poco/Timestamp.h"
#include "Poco/Timezone.h"
#include "Poco/Environment.h"
#include <cstdio>
#include <cctype>


namespace Poco {


const std::string PatternFormatter::PROP_PATTERN = "pattern";
const std::string PatternFormatter::PROP_TIMES   = "times";


PatternFormatter::PatternFormatter():
	_localTime(false)
{
}


PatternFormatter::PatternFormatter(const std::string& format):
	_localTime(false),
	_pattern(format)
{
}


PatternFormatter::~PatternFormatter()
{
}


inline void PatternFormatter::fmt(std::string& str, int value)
{
        char buffer[64];
        std::sprintf(buffer, "%d", value);
        str.append(buffer);
}


inline void PatternFormatter::fmt(std::string& str, int value, int width)
{
        char buffer[64];
        std::sprintf(buffer, "%*d", width, value);
        str.append(buffer);
}


inline void PatternFormatter::fmt0(std::string& str, int value, int width)
{
        char buffer[64];
        std::sprintf(buffer, "%0*d", width, value);
        str.append(buffer);
}


void PatternFormatter::format(const Message& msg, std::string& text)
{
        Timestamp timestamp = msg.getTime();
	if (_localTime)
	{
		timestamp += Timezone::utcOffset()*Timestamp::resolution();
		timestamp += Timezone::dst()*Timestamp::resolution();
	}
	DateTime dateTime = timestamp;
	std::string::const_iterator it  = _pattern.begin();
	std::string::const_iterator end = _pattern.end();
	while (it != end)
	{
		if (*it == '%')
		{
			if (++it != end)
			{
				switch (*it)
                                {
                                case 's': text.append(msg.getSource()); break;
                                case 't': text.append(msg.getText()); break;
                                case 'l': NumberFormatter::append(text, (int) msg.getPriority()); break;
                                case 'p': text.append(getPriorityName((int) msg.getPriority())); break;
                                case 'q': text += getPriorityName((int) msg.getPriority()).at(0); break;
                                case 'P': NumberFormatter::append(text, msg.getPid()); break;
                                case 'T': text.append(msg.getThread()); break;
                                case 'I': NumberFormatter::append(text, msg.getTid()); break;
                                case 'N': text.append(Environment::nodeName()); break;
                                case 'U': text.append(msg.getSourceFile() ? msg.getSourceFile() : ""); break;
                                case 'u': NumberFormatter::append(text, msg.getSourceLine()); break;
                                case 'w': text.append(DateTimeFormat::WEEKDAY_NAMES[dateTime.dayOfWeek()], 0, 3); break;
                                case 'W': text.append(DateTimeFormat::WEEKDAY_NAMES[dateTime.dayOfWeek()]); break;
                                case 'b': text.append(DateTimeFormat::MONTH_NAMES[dateTime.month() - 1], 0, 3); break;
                                case 'B': text.append(DateTimeFormat::MONTH_NAMES[dateTime.month() - 1]); break;
                                case 'd': NumberFormatter::append0(text, dateTime.day(), 2); break;
                                case 'e': NumberFormatter::append(text, dateTime.day()); break;
                                case 'f': NumberFormatter::append(text, dateTime.day(), 2); break;
                                case 'm': NumberFormatter::append0(text, dateTime.month(), 2); break;
                                case 'n': NumberFormatter::append(text, dateTime.month()); break;
                                case 'o': NumberFormatter::append(text, dateTime.month(), 2); break;
                                case 'y': NumberFormatter::append0(text, dateTime.year() % 100, 2); break;
                                case 'Y': NumberFormatter::append0(text, dateTime.year(), 4); break;
                                case 'H': NumberFormatter::append0(text, dateTime.hour(), 2); break;
                                case 'h': NumberFormatter::append0(text, dateTime.hourAMPM(), 2); break;
                                case 'a': text.append(dateTime.isAM() ? "am" : "pm"); break;
                                case 'A': text.append(dateTime.isAM() ? "AM" : "PM"); break;
                                case 'M': NumberFormatter::append0(text, dateTime.minute(), 2); break;
                                case 'S': NumberFormatter::append0(text, dateTime.second(), 2); break;
                                case 'i': NumberFormatter::append0(text, dateTime.millisecond(), 3); break;
                                case 'c': NumberFormatter::append(text, dateTime.millisecond()/100); break;
                                case 'F': NumberFormatter::append0(text, dateTime.millisecond()*1000 + dateTime.microsecond(), 6); break;
                                case 'z': text.append(DateTimeFormatter::tzdISO(_localTime ? Timezone::tzd() : DateTimeFormatter::UTC)); break;
                                case 'Z': text.append(DateTimeFormatter::tzdRFC(_localTime ? Timezone::tzd() : DateTimeFormatter::UTC)); break;
                                case 'E': NumberFormatter::append(text, msg.getTime().epochTime()); break;
                                case '[':
                                {
                                        ++it;
					std::string prop;
					while (it != end && *it != ']') prop += *it++;
					if (it == end) --it;
					try
					{
						text.append(msg[prop]);
					}
					catch (...)
					{
					}
					break;
				}
				default: text += *it;
				}
				++it;
			}
		}
		else text += *it++;
	}
}

	
void PatternFormatter::setProperty(const std::string& name, const std::string& value)
{
	if (name == PROP_PATTERN)
		_pattern = value;
	else if (name == PROP_TIMES)
		_localTime = (value == "local");
	else 
		Formatter::setProperty(name, value);
}


std::string PatternFormatter::getProperty(const std::string& name) const
{
	if (name == PROP_PATTERN)
		return _pattern;
	else if (name == PROP_TIMES)
		return _localTime ? "local" : "UTC";
	else
		return Formatter::getProperty(name);
}


namespace
{
        static std::string priorities[] = 
        {
		"",
		"Fatal",
		"Critical",
		"Error",
		"Warning",
		"Notice",
		"Information",
                "Debug",
                "Trace"
        };
}


const std::string& PatternFormatter::getPriorityName(int prio)
{
        poco_assert (1 <= prio && prio <= 8);   
        return priorities[prio];
}


} // namespace Poco
