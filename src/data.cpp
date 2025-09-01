/* data.cpp -  static global data

Copyright (c) 2013 P. Vorpahl

This file is part of Grepotool, a free utility for the browsergame 
Grepolis which is the intellectual property of InnoGames GmbH.

Grepotool is free Software: you can redistribute it and/or modify 
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the 
License, or any later version. Alternatively, you may use it under 
the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or any later 
version.

Grepotool is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with 'Grepotool; see the files 'GPL.txt' and 'LGPL.txt' in 
the base directory of your installation. If not, see 
'http://www.gnu.org/licenses/'.
*/

#include "defines.h"
#include "json.h"
#include "zlib.h"

#include <QStringList>
#include <QFile>
#include <QByteArray>

QByteArray readGzipfile (const QString& filename, bool& err)
{
	QByteArray fn = filename.toLatin1();
	QByteArray r;
	gzFile zf = gzopen(fn.constData(), "rb");
	if (!zf) {
		err = true;
		return r;
	}
	char buff[10000];
	int b = gzread(zf,buff,10000);
	while(b > 0) {
		r.append(buff,b);
		if (b < 10000) break;
		b = gzread(zf,buff,10000);
	}
	if (b<0) {
		err = true;
		gzclose (zf);
		return r;
	}
	gzclose (zf);		
	return r;
}

QByteArray readZipfile (const QString& filename, bool& err, QString &msg)
{
	QByteArray ret;
	QFile f(filename);
	err= false;
	if (!f.open(QIODevice::ReadOnly)) {
		err = true;
		msg = f.errorString();
		return ret;
	}
	ret = f.readAll ();
	ret = qUncompress(ret);
	return ret;
}
bool writeZipfile (const QString& filename, const QByteArray &d)
{
	QByteArray dn = qCompress(d);
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly)) return false;
	f.write (dn);
	return true;
}


QColor randomColor () {
	int r = qrand() & 255;
	int g = qrand() & 255;
	int b = qrand() & 255;	
	return QColor(r,g,b,255);
};
static QVector <QPoint> town_offsets[61];
static QVector <int> town_orientations[61];
static QVector <int> town_label_directions[61];
static QPoint island_center_offsets [61];

static void initOffsetTable () {

	QFile f(":/data/offsets.json");
	f.open(QIODevice::ReadOnly);
	QString tx = f.readAll ();
	bool succ;
	QVariantMap m = QtJson::parse (tx,succ).toMap();

	m = m["islands"].toMap ();

	for (int i=1;  i < 61; ++i) {
		QString in = QString::number(i);
		if(m.contains(in)) {
			QVariantMap il = m[in].toMap();
			QVariantMap sl = il["slots"].toMap();

			int ilx = il["centering_offset_x"].toInt();
			int ily = il["centering_offset_y"].toInt();

			island_center_offsets [i] = QPoint (ilx,ily);
			for (int n=0;n<20;++n) {
				QString slotno = QString::number(n);
				if (!sl.contains(slotno)) break;
				QVariantMap slinf = sl[slotno].toMap ();
				int x = slinf["x"].toInt()+50;
				int y = slinf["y"].toInt()+32;
				int to = slinf["orientation"].toInt();
				int lp = slinf["label_dir"].toInt();
				town_offsets[i].push_back (QPoint(x,y));	
				town_label_directions[i].push_back(lp);
				town_orientations[i].push_back(to);
			}
		}
	}

	return;
}

QPoint getIslandCenterOffset (int island) { return island_center_offsets [island];};

QPoint getTownOffset (int island, int town_no) {

	if (! town_offsets[1].size()) 
		initOffsetTable ();

	if (island<1 || island>60)  return QPoint(0,0);
	if (town_no >= town_offsets[island].size()) return QPoint(0,0);
	return town_offsets[island][town_no];

}

int getTownOrientation (int island, int town_no) {

	if (! town_offsets[1].size()) 
		initOffsetTable ();

	if (island<1 || island>60)  return -1;
	if (town_no<0 ||town_no >= town_offsets[island].size()) return -1;
	return town_orientations[island][town_no];
}

int getTownLabelDirection (int island, int town_no) {

	if (! town_offsets[1].size()) 
		initOffsetTable ();

	if (island<1 || island>60)  return -1;
	if (town_no<0 ||town_no >= town_offsets[island].size()) return -1;
	return town_label_directions[island][town_no];
}

static int char2hex (QChar a) {
	if(a == '0') return 0;
	if(a == '1') return 1;
	if(a == '2') return 2;
	if(a == '3') return 3;
	if(a == '4') return 4;
	if(a == '5') return 5;
	if(a == '6') return 6;
	if(a == '7') return 7;
	if(a == '8') return 8;
	if(a == '9') return 9;
	if(a == 'A') return 10;
	if(a == 'B') return 11;
	if(a == 'C') return 12;
	if(a == 'D') return 13;
	if(a == 'E') return 14;
	if(a == 'F') return 15;

	return 0;
}


QString replaceStrangeChars (const QString &s) 
{
	QString r;
	int i=0;


	while (i < s.length()) {
		if (s.at(i) == '%') {
			++i;
			int m = 16* char2hex(s.at(i));
			++i;
			m += char2hex(s.at(i));
			++i;
			if (i< s.length() && s.at(i) == '%') {
				++i;
				int mm = 16* char2hex(s.at(i));
				++i;
				mm += char2hex(s.at(i));
				++i;
				char ba [3];
				ba[0] = m;
				ba[1] = mm;
				ba[2] = 0;
				r += QString::fromUtf8 (ba,2);
			} else {
				r += QChar(m);
			}
		} else if(s.at(i) == '+') {
			r += " ";
		++i;
		} else {
			r += s.at(i);
		++i;
		}
	}

	return r;
}












/*
	Islands with villages
	town_offsets[1].push_back (QPoint(500,173));	
	town_offsets[1].push_back (QPoint(400,204));	
	town_offsets[1].push_back (QPoint(291,278));	
	town_offsets[1].push_back (QPoint(235,307));	
	town_offsets[1].push_back (QPoint(244,354));	
	town_offsets[1].push_back (QPoint(273,429));	
	town_offsets[1].push_back (QPoint(338,425));	
	town_offsets[1].push_back (QPoint(408,435));	
	town_offsets[1].push_back (QPoint(503,501));	
	town_offsets[1].push_back (QPoint(564,533));	
	town_offsets[1].push_back (QPoint(651,495));	
	town_offsets[1].push_back (QPoint(758,496));	
	town_offsets[1].push_back (QPoint(799,475));	
	town_offsets[1].push_back (QPoint(874,443));	
	town_offsets[1].push_back (QPoint(930,387));	
	town_offsets[1].push_back (QPoint(917,328));	
	town_offsets[1].push_back (QPoint(850,235));	
	town_offsets[1].push_back (QPoint(777,262));	
	town_offsets[1].push_back (QPoint(822,320));	
	town_offsets[1].push_back (QPoint(794,360));	

	town_offsets[2].push_back (QPoint(360,222));	
	town_offsets[2].push_back (QPoint(366,136));	
	town_offsets[2].push_back (QPoint(447,143));	
	town_offsets[2].push_back (QPoint(542,111));	
	town_offsets[2].push_back (QPoint(672,116));	
	town_offsets[2].push_back (QPoint(734,140));	
	town_offsets[2].push_back (QPoint(794,173));	
	town_offsets[2].push_back (QPoint(832,204));	
	town_offsets[2].push_back (QPoint(816,272));	
	town_offsets[2].push_back (QPoint(778,336));	
	town_offsets[2].push_back (QPoint(786,400));	
	town_offsets[2].push_back (QPoint(731,420));	
	town_offsets[2].push_back (QPoint(661,451));	
	town_offsets[2].push_back (QPoint(539,444));	
	town_offsets[2].push_back (QPoint(335,429));	
	town_offsets[2].push_back (QPoint(273,427));	
	town_offsets[2].push_back (QPoint(247,364));	
	town_offsets[2].push_back (QPoint(178,324));	
	town_offsets[2].push_back (QPoint(143,266));	
	town_offsets[2].push_back (QPoint(223,224));	

	town_offsets[3].push_back (QPoint(532,114));	
	town_offsets[3].push_back (QPoint(456, 98));	
	town_offsets[3].push_back (QPoint(393,116));	
	town_offsets[3].push_back (QPoint(336,142));	
	town_offsets[3].push_back (QPoint(286,181));	
	town_offsets[3].push_back (QPoint(196,250));	
	town_offsets[3].push_back (QPoint(187,349));	
	town_offsets[3].push_back (QPoint(275,392));	
	town_offsets[3].push_back (QPoint(345,424));	
	town_offsets[3].push_back (QPoint(400,441));	
	town_offsets[3].push_back (QPoint(458,478));	
	town_offsets[3].push_back (QPoint(545,490));	
	town_offsets[3].push_back (QPoint(611,463));	
	town_offsets[3].push_back (QPoint(644,414));	
	town_offsets[3].push_back (QPoint(776,415));	
	town_offsets[3].push_back (QPoint(753,381));	
	town_offsets[3].push_back (QPoint(906,281));	
	town_offsets[3].push_back (QPoint(901,230));	
	town_offsets[3].push_back (QPoint(818,180));	
	town_offsets[3].push_back (QPoint(736,133));	

	town_offsets[4].push_back (QPoint(564,149));	
	town_offsets[4].push_back (QPoint(632,173));	
	town_offsets[4].push_back (QPoint(728,180));	
	town_offsets[4].push_back (QPoint(765,211));	
	town_offsets[4].push_back (QPoint(814,241));	
	town_offsets[4].push_back (QPoint(732,280));	
	town_offsets[4].push_back (QPoint(658,311));	
	town_offsets[4].push_back (QPoint(574,319));	
	town_offsets[4].push_back (QPoint(519,269));	
	town_offsets[4].push_back (QPoint(441,236));	
	town_offsets[4].push_back (QPoint(373,238));
	town_offsets[4].push_back (QPoint(268,272));	
	town_offsets[4].push_back (QPoint(314,316));	
	town_offsets[4].push_back (QPoint(318,354));	
	town_offsets[4].push_back (QPoint(247,389));	
	town_offsets[4].push_back (QPoint(155,373));	
	town_offsets[4].push_back (QPoint(152,295));	
	town_offsets[4].push_back (QPoint(184,243));	
	town_offsets[4].push_back (QPoint(296,196));	
	town_offsets[4].push_back (QPoint(380,166));	

	town_offsets[5].push_back (QPoint(259,246));	
	town_offsets[5].push_back (QPoint(262,172));	
	town_offsets[5].push_back (QPoint(368,121));	
	town_offsets[5].push_back (QPoint(458, 87));	
	town_offsets[5].push_back (QPoint(569,102));	
	town_offsets[5].push_back (QPoint(581,161));	
	town_offsets[5].push_back (QPoint(692,163));	
	town_offsets[5].push_back (QPoint(782,162));	
	town_offsets[5].push_back (QPoint(783,214));	
	town_offsets[5].push_back (QPoint(887,233));	
	town_offsets[5].push_back (QPoint(894,294));	
	town_offsets[5].push_back (QPoint(834,367));	
	town_offsets[5].push_back (QPoint(773,403));	
	town_offsets[5].push_back (QPoint(692,433));	
	town_offsets[5].push_back (QPoint(590,444));	
	town_offsets[5].push_back (QPoint(490,509));	
	town_offsets[5].push_back (QPoint(375,459));	
	town_offsets[5].push_back (QPoint(321,403));	
	town_offsets[5].push_back (QPoint(654,353));	
	town_offsets[5].push_back (QPoint(679,277));	

	town_offsets[6].push_back (QPoint(512,407));	
	town_offsets[6].push_back (QPoint(594,459));	
	town_offsets[6].push_back (QPoint(733,488));	
	town_offsets[6].push_back (QPoint(825,502));	
	town_offsets[6].push_back (QPoint(874,462));	
	town_offsets[6].push_back (QPoint(880,391));	
	town_offsets[6].push_back (QPoint(807,356));	
	town_offsets[6].push_back (QPoint(741,324));	
	town_offsets[6].push_back (QPoint(702,252));	
	town_offsets[6].push_back (QPoint(641,222));	
	town_offsets[6].push_back (QPoint(545,180));	
	town_offsets[6].push_back (QPoint(462,180));	
	town_offsets[6].push_back (QPoint(385,200));	
	town_offsets[6].push_back (QPoint(310,213));	
	town_offsets[6].push_back (QPoint(224,265));	
	town_offsets[6].push_back (QPoint(140,326));	
	town_offsets[6].push_back (QPoint(194,364));	
	town_offsets[6].push_back (QPoint(227,407));	
	town_offsets[6].push_back (QPoint(219,454));	
	town_offsets[6].push_back (QPoint(317,478));	

	town_offsets[7].push_back (QPoint(191,282));	
	town_offsets[7].push_back (QPoint(177,248));	
	town_offsets[7].push_back (QPoint(156,198));	
	town_offsets[7].push_back (QPoint(161,149));	
	town_offsets[7].push_back (QPoint(220,130));	
	town_offsets[7].push_back (QPoint(279,154));	
	town_offsets[7].push_back (QPoint(362,182));	
	town_offsets[7].push_back (QPoint(430,209));	
	town_offsets[7].push_back (QPoint(551,225));	
	town_offsets[7].push_back (QPoint(492,255));	
	town_offsets[7].push_back (QPoint(529,306));	
	town_offsets[7].push_back (QPoint(555,349));	
	town_offsets[7].push_back (QPoint(552,394));	
	town_offsets[7].push_back (QPoint(486,425));	
	town_offsets[7].push_back (QPoint(481,496));	
	town_offsets[7].push_back (QPoint(409,478));	
	town_offsets[7].push_back (QPoint(346,456));	
	town_offsets[7].push_back (QPoint(265,485));	
	town_offsets[7].push_back (QPoint(223,449));	
	town_offsets[7].push_back (QPoint(163,431));	

	town_offsets[8].push_back (QPoint(608,277));	
	town_offsets[8].push_back (QPoint(685,306));	
	town_offsets[8].push_back (QPoint(759,332));	
	town_offsets[8].push_back (QPoint(781,372));	
	town_offsets[8].push_back (QPoint(736,398));	
	town_offsets[8].push_back (QPoint(692,429));	
	town_offsets[8].push_back (QPoint(622,447));	
	town_offsets[8].push_back (QPoint(517,475));	
	town_offsets[8].push_back (QPoint(439,472));	
	town_offsets[8].push_back (QPoint(348,457));	
	town_offsets[8].push_back (QPoint(280,429));	
	town_offsets[8].push_back (QPoint(230,377));	
	town_offsets[8].push_back (QPoint(186,345));	
	town_offsets[8].push_back (QPoint(181,295));	
	town_offsets[8].push_back (QPoint(145,251));	
	town_offsets[8].push_back (QPoint(147,208));	
	town_offsets[8].push_back (QPoint(213,185));	
	town_offsets[8].push_back (QPoint(272,164));	
	town_offsets[8].push_back (QPoint(351,156));	
	town_offsets[8].push_back (QPoint(388,202));	

	town_offsets[9].push_back (QPoint(235,369));	
	town_offsets[9].push_back (QPoint(165,326));	
	town_offsets[9].push_back (QPoint(174,267));	
	town_offsets[9].push_back (QPoint(229,202));	
	town_offsets[9].push_back (QPoint(391,117));	
	town_offsets[9].push_back (QPoint(466,86));	
	town_offsets[9].push_back (QPoint(559,99));	
	town_offsets[9].push_back (QPoint(624,153));	
	town_offsets[9].push_back (QPoint(856,227));	
	town_offsets[9].push_back (QPoint(897,270));	
	town_offsets[9].push_back (QPoint(877,357));	
	town_offsets[9].push_back (QPoint(753,429));	
	town_offsets[9].push_back (QPoint(688,461));	
	town_offsets[9].push_back (QPoint(656,399));	
	town_offsets[9].push_back (QPoint(562,427));	
	town_offsets[9].push_back (QPoint(573,481));	
	town_offsets[9].push_back (QPoint(489,477));	
	town_offsets[9].push_back (QPoint(417,478));	
	town_offsets[9].push_back (QPoint(392,434));	
	town_offsets[9].push_back (QPoint(364,399));	

	town_offsets[10].push_back (QPoint(606,180));	
	town_offsets[10].push_back (QPoint(650,143));	
	town_offsets[10].push_back (QPoint(747,145));	
	town_offsets[10].push_back (QPoint(821,193));	
	town_offsets[10].push_back (QPoint(869,251));	
	town_offsets[10].push_back (QPoint(849,355));	
	town_offsets[10].push_back (QPoint(794,392));	
	town_offsets[10].push_back (QPoint(714,430));	
	town_offsets[10].push_back (QPoint(545,450));	
	town_offsets[10].push_back (QPoint(451,468));	
	town_offsets[10].push_back (QPoint(353,443));	
	town_offsets[10].push_back (QPoint(302,413));	
	town_offsets[10].push_back (QPoint(238,386));	
	town_offsets[10].push_back (QPoint(181,327));	
	town_offsets[10].push_back (QPoint(191,222));	
	town_offsets[10].push_back (QPoint(275,155));	
	town_offsets[10].push_back (QPoint(378,140));	
	town_offsets[10].push_back (QPoint(425,186));	
	town_offsets[10].push_back (QPoint(397,232));	
	town_offsets[10].push_back (QPoint(304,238));	

	// 37
	town_offsets[37].push_back (QPoint(547,161));	
	town_offsets[37].push_back (QPoint(432,187));	
	town_offsets[37].push_back (QPoint(488,227));	
	town_offsets[37].push_back (QPoint(623,325));	
	town_offsets[37].push_back (QPoint(597,386));	
	town_offsets[37].push_back (QPoint(417,260));	
	town_offsets[37].push_back (QPoint(334,229));	
	town_offsets[37].push_back (QPoint(244,276));	
	town_offsets[37].push_back (QPoint(178,352));	
	town_offsets[37].push_back (QPoint(223,396));	
	town_offsets[37].push_back (QPoint(287,443));	
	town_offsets[37].push_back (QPoint(434,490));	
	town_offsets[37].push_back (QPoint(621,535));	
	town_offsets[37].push_back (QPoint(693,532));	
	town_offsets[37].push_back (QPoint(747,495));	
	town_offsets[37].push_back (QPoint(820,485));	
	town_offsets[37].push_back (QPoint(904,399));	
	town_offsets[37].push_back (QPoint(904,281));	
	town_offsets[37].push_back (QPoint(818,239));	
	town_offsets[37].push_back (QPoint(735,193));	

	town_offsets[38].push_back (QPoint(390,117));	
	town_offsets[38].push_back (QPoint(293, 88));	
	town_offsets[38].push_back (QPoint(201,124));	
	town_offsets[38].push_back (QPoint(140,183));	
	town_offsets[38].push_back (QPoint(137,239));	
	town_offsets[38].push_back (QPoint(120,281));	
	town_offsets[38].push_back (QPoint(161,332));	
	town_offsets[38].push_back (QPoint(273,354));	
	town_offsets[38].push_back (QPoint(343,398));	
	town_offsets[38].push_back (QPoint(436,418));	
	town_offsets[38].push_back (QPoint(501,407));	
	town_offsets[38].push_back (QPoint(573,409));	
	town_offsets[38].push_back (QPoint(656,393));	
	town_offsets[38].push_back (QPoint(721,373));	
	town_offsets[38].push_back (QPoint(799,344));	
	town_offsets[38].push_back (QPoint(718,254));	
	town_offsets[38].push_back (QPoint(847,237));	
	town_offsets[38].push_back (QPoint(778,134));	
	town_offsets[38].push_back (QPoint(680, 93));	
	town_offsets[38].push_back (QPoint(586, 88));	

	town_offsets[39].push_back (QPoint(566, 83));	
	town_offsets[39].push_back (QPoint(470,116));	
	town_offsets[39].push_back (QPoint(297,155));	
	town_offsets[39].push_back (QPoint(225,224));	
	town_offsets[39].push_back (QPoint(178,277));	
	town_offsets[39].push_back (QPoint(151,339));	
	town_offsets[39].push_back (QPoint(235,406));	
	town_offsets[39].push_back (QPoint(304,443));	
	town_offsets[39].push_back (QPoint(387,448));	
	town_offsets[39].push_back (QPoint(468,423));	
	town_offsets[39].push_back (QPoint(559,426));	
	town_offsets[39].push_back (QPoint(694,443));	
	town_offsets[39].push_back (QPoint(769,408));	
	town_offsets[39].push_back (QPoint(868,374));	
	town_offsets[39].push_back (QPoint(928,301));	
	town_offsets[39].push_back (QPoint(732,285));	
	town_offsets[39].push_back (QPoint(674,245));	
	town_offsets[39].push_back (QPoint(753,229));	
	town_offsets[39].push_back (QPoint(715,142));	
	town_offsets[39].push_back (QPoint(641,102));	

	town_offsets[40].push_back (QPoint(434,134));	
	town_offsets[40].push_back (QPoint(392,190));	
	town_offsets[40].push_back (QPoint(381,249));	
	town_offsets[40].push_back (QPoint(221,273));
	town_offsets[40].push_back (QPoint(146,323));	
	town_offsets[40].push_back (QPoint(190,383));	
	town_offsets[40].push_back (QPoint(246,416));	
	town_offsets[40].push_back (QPoint(327,439));	
	town_offsets[40].push_back (QPoint(398,470));	
	town_offsets[40].push_back (QPoint(485,439));	
	town_offsets[40].push_back (QPoint(686,466));	
	town_offsets[40].push_back (QPoint(765,427));	
	town_offsets[40].push_back (QPoint(828,380));	
	town_offsets[40].push_back (QPoint(905,348));	
	town_offsets[40].push_back (QPoint(900,293));	
	town_offsets[40].push_back (QPoint(797,283));	
	town_offsets[40].push_back (QPoint(724,260));	
	town_offsets[40].push_back (QPoint(708,179));	
	town_offsets[40].push_back (QPoint(591,164));	
	town_offsets[40].push_back (QPoint(518,119));	

	town_offsets[41].push_back (QPoint(489,133));	
	town_offsets[41].push_back (QPoint(403,151));		
	town_offsets[41].push_back (QPoint(298,185));	
	town_offsets[41].push_back (QPoint(226,228));
	town_offsets[41].push_back (QPoint(194,264));	
	town_offsets[41].push_back (QPoint(197,313));	
	town_offsets[41].push_back (QPoint(205,359));	
	town_offsets[41].push_back (QPoint(242,413));	
	town_offsets[41].push_back (QPoint(308,445));
	town_offsets[41].push_back (QPoint(377,467));	
	town_offsets[41].push_back (QPoint(511,500));	
	town_offsets[41].push_back (QPoint(636,481));	
	town_offsets[41].push_back (QPoint(770,422));	
	town_offsets[41].push_back (QPoint(842,383));	
	town_offsets[41].push_back (QPoint(916,358));	
	town_offsets[41].push_back (QPoint(926,301));	
	town_offsets[41].push_back (QPoint(821,280));	
	town_offsets[41].push_back (QPoint(711,336));	
	town_offsets[41].push_back (QPoint(676,172));	
	town_offsets[41].push_back (QPoint(568,125));	

	town_offsets[42].push_back (QPoint(520,119));	
	town_offsets[42].push_back (QPoint(438,129));	
	town_offsets[42].push_back (QPoint(370,168));		
	town_offsets[42].push_back (QPoint(278,193));	
	town_offsets[42].push_back (QPoint(205,249));	
	town_offsets[42].push_back (QPoint(167,341));
	town_offsets[42].push_back (QPoint(176,384));	
	town_offsets[42].push_back (QPoint(230,416));	
	town_offsets[42].push_back (QPoint(260,513));	
	town_offsets[42].push_back (QPoint(363,522));	
	town_offsets[42].push_back (QPoint(371,408));	
	town_offsets[42].push_back (QPoint(540,444));	
	town_offsets[42].push_back (QPoint(539,488));	
	town_offsets[42].push_back (QPoint(653,481));	
	town_offsets[42].push_back (QPoint(715,446));	
	town_offsets[42].push_back (QPoint(827,399));	
	town_offsets[42].push_back (QPoint(884,308));	
	town_offsets[42].push_back (QPoint(754,195));	
	town_offsets[42].push_back (QPoint(685,161));	
	town_offsets[42].push_back (QPoint(603,132));	

	town_offsets[43].push_back (QPoint(356, 36));	
	town_offsets[43].push_back (QPoint(299,55));	
	town_offsets[43].push_back (QPoint(230,114));	
	town_offsets[43].push_back (QPoint(130,142));
	town_offsets[43].push_back (QPoint(142,206));	
	town_offsets[43].push_back (QPoint(162,257));	
	town_offsets[43].push_back (QPoint(158,304));	
	town_offsets[43].push_back (QPoint(229,193));	
	town_offsets[43].push_back (QPoint(701,232));	
	town_offsets[43].push_back (QPoint(347,341));	
	town_offsets[43].push_back (QPoint(412,370));
	town_offsets[43].push_back (QPoint(497,344));	
	town_offsets[43].push_back (QPoint(603,346));	
	town_offsets[43].push_back (QPoint(704,333));	
	town_offsets[43].push_back (QPoint(764,293));	
	town_offsets[43].push_back (QPoint(816,262));	
	town_offsets[43].push_back (QPoint(797,129));	
	town_offsets[43].push_back (QPoint(737, 72));	
	town_offsets[43].push_back (QPoint(580, 82));	
	town_offsets[43].push_back (QPoint(504, 84));	

	town_offsets[44].push_back (QPoint(588, 54));	
	town_offsets[44].push_back (QPoint(557,102));	
	town_offsets[44].push_back (QPoint(468,75));	
	town_offsets[44].push_back (QPoint(355,73));	
	town_offsets[44].push_back (QPoint(392,139));	
	town_offsets[44].push_back (QPoint(210,171));	
	town_offsets[44].push_back (QPoint(224,244));	
	town_offsets[44].push_back (QPoint(281,312));	
	town_offsets[44].push_back (QPoint(391,335));	
	town_offsets[44].push_back (QPoint(471,392));	
	town_offsets[44].push_back (QPoint(526,291));	
	town_offsets[44].push_back (QPoint(605,290));	
	town_offsets[44].push_back (QPoint(644,401));	
	town_offsets[44].push_back (QPoint(744,365));	
	town_offsets[44].push_back (QPoint(833,334));	
	town_offsets[44].push_back (QPoint(834,257));	
	town_offsets[44].push_back (QPoint(923,212));	
	town_offsets[44].push_back (QPoint(865,155));	
	town_offsets[44].push_back (QPoint(729,126));	
	town_offsets[44].push_back (QPoint(754,66));	

	town_offsets[45].push_back (QPoint(396,174));	
	town_offsets[45].push_back (QPoint(336,225));	
	town_offsets[45].push_back (QPoint(212,238));	
	town_offsets[45].push_back (QPoint(249,285));	
	town_offsets[45].push_back (QPoint(310,357));	
	town_offsets[45].push_back (QPoint(234,366));	
	town_offsets[45].push_back (QPoint(154,392));	
	town_offsets[45].push_back (QPoint(158,445));	
	town_offsets[45].push_back (QPoint(236,471));	
	town_offsets[45].push_back (QPoint(337,457));	
	town_offsets[45].push_back (QPoint(466,423));	
	town_offsets[45].push_back (QPoint(509,484));	
	town_offsets[45].push_back (QPoint(644,484));	
	town_offsets[45].push_back (QPoint(715,433));	
	town_offsets[45].push_back (QPoint(778,378));	
	town_offsets[45].push_back (QPoint(807,292));	
	town_offsets[45].push_back (QPoint(703,175));	
	town_offsets[45].push_back (QPoint(625,175));	
	town_offsets[45].push_back (QPoint(548,188));	
	town_offsets[45].push_back (QPoint(471,142));	

	town_offsets[46].push_back (QPoint(547,117));	
	town_offsets[46].push_back (QPoint(386,144));	
	town_offsets[46].push_back (QPoint(341,191));	
	town_offsets[46].push_back (QPoint(265,207));	
	town_offsets[46].push_back (QPoint(205,238));	
	town_offsets[46].push_back (QPoint(144,287));	
	town_offsets[46].push_back (QPoint(91 ,344));	
	town_offsets[46].push_back (QPoint(151,373));	
	town_offsets[46].push_back (QPoint(219,465));	
	town_offsets[46].push_back (QPoint(318,451));	
	town_offsets[46].push_back (QPoint(356,512));	
	town_offsets[46].push_back (QPoint(564,510));	
	town_offsets[46].push_back (QPoint(635,464));	
	town_offsets[46].push_back (QPoint(738,440));	
	town_offsets[46].push_back (QPoint(748,413));
	town_offsets[46].push_back (QPoint(776,340));	
	town_offsets[46].push_back (QPoint(777,274));	
	town_offsets[46].push_back (QPoint(740,228));	
	town_offsets[46].push_back (QPoint(688,199));
	town_offsets[46].push_back (QPoint(626,133));	

	// Uninhabited islands
	town_offsets[11].push_back (QPoint(167,159));	
	town_offsets[11].push_back (QPoint(241,111));	
	town_offsets[11].push_back (QPoint(321,115));	
	town_offsets[11].push_back (QPoint(442,181));	
	town_offsets[11].push_back (QPoint(465,267));	
	town_offsets[11].push_back (QPoint(359,209));	
	town_offsets[11].push_back (QPoint(304,157));	

	town_offsets[12].push_back (QPoint(224,109));	
	town_offsets[12].push_back (QPoint(330,128));	
	town_offsets[12].push_back (QPoint(373,223));	
	town_offsets[12].push_back (QPoint(313,243));	
	town_offsets[12].push_back (QPoint(245,265));	
	town_offsets[12].push_back (QPoint(152,240));	
	town_offsets[12].push_back (QPoint(179,188));	

	town_offsets[13].push_back (QPoint(444, 42));	
	town_offsets[13].push_back (QPoint(420,182));	
	town_offsets[13].push_back (QPoint(593,328));	
	town_offsets[13].push_back (QPoint(514,333));	
	town_offsets[13].push_back (QPoint(451,308));	
	town_offsets[13].push_back (QPoint(374,340));	
	town_offsets[13].push_back (QPoint(283,303));	
	town_offsets[13].push_back (QPoint(222,269));	
	town_offsets[13].push_back (QPoint(251,173));	
	town_offsets[13].push_back (QPoint(248,83));	
	town_offsets[13].push_back (QPoint(346,63));	

	town_offsets[14].push_back (QPoint(240,109));	
	town_offsets[14].push_back (QPoint(348,119));	
	town_offsets[14].push_back (QPoint(346,174));	
	town_offsets[14].push_back (QPoint(301,217));	
	town_offsets[14].push_back (QPoint(286,278));	
	town_offsets[14].push_back (QPoint(188,269));	
	town_offsets[14].push_back (QPoint(117,235));	

	town_offsets[15].push_back (QPoint(163,123));	
	town_offsets[15].push_back (QPoint(284,132));	
	town_offsets[15].push_back (QPoint(316,180));	
	town_offsets[15].push_back (QPoint(413,165));	
	town_offsets[15].push_back (QPoint(474,200));	
	town_offsets[15].push_back (QPoint(534,246));	
	town_offsets[15].push_back (QPoint(464,302));	
	town_offsets[15].push_back (QPoint(369,311));	

	town_offsets[16].push_back (QPoint(157,153));	
	town_offsets[16].push_back (QPoint(241,146));	
	town_offsets[16].push_back (QPoint(317,145));	
	town_offsets[16].push_back (QPoint(404,215));	
	town_offsets[16].push_back (QPoint(498,241));	
	town_offsets[16].push_back (QPoint(604,254));	
	town_offsets[16].push_back (QPoint(661,290));	
	town_offsets[16].push_back (QPoint(645,340));	
	town_offsets[16].push_back (QPoint(559,357));	
	town_offsets[16].push_back (QPoint(388,332));	
	town_offsets[16].push_back (QPoint(276,307));	
	town_offsets[16].push_back (QPoint(227,250));	
	town_offsets[16].push_back (QPoint(163,221));	

	town_offsets[47].push_back (QPoint(391,181));	
	town_offsets[47].push_back (QPoint(306,146));	
	town_offsets[47].push_back (QPoint(203,175));	
	town_offsets[47].push_back (QPoint(117,219));	
	town_offsets[47].push_back (QPoint(314,255));	
	town_offsets[47].push_back (QPoint(544,354));	
	town_offsets[47].push_back (QPoint(577,319));	
	town_offsets[47].push_back (QPoint(526,267));	
	town_offsets[47].push_back (QPoint(489,191));	

	town_offsets[48].push_back (QPoint(192,244));	
	town_offsets[48].push_back (QPoint(250,266));	
	town_offsets[48].push_back (QPoint(327,258));	
	town_offsets[48].push_back (QPoint(378,221));	
	town_offsets[48].push_back (QPoint(439,172));	
	town_offsets[48].push_back (QPoint(506,148));	
	town_offsets[48].push_back (QPoint(446,116));	
	town_offsets[48].push_back (QPoint(413, 31));	

	town_offsets[49].push_back (QPoint(136,142));	
	town_offsets[49].push_back (QPoint(282,146));	
	town_offsets[49].push_back (QPoint(375,129));	
	town_offsets[49].push_back (QPoint(524,160));	
	town_offsets[49].push_back (QPoint(751,116));	
	town_offsets[49].push_back (QPoint(719,169));	
	town_offsets[49].push_back (QPoint(516,284));	
	town_offsets[49].push_back (QPoint(356,302));	
	town_offsets[49].push_back (QPoint(283,290));	

	town_offsets[50].push_back (QPoint(192,153));	
	town_offsets[50].push_back (QPoint(254,134));	
	town_offsets[50].push_back (QPoint(322,149));	
	town_offsets[50].push_back (QPoint(398,171));	
	town_offsets[50].push_back (QPoint(459,209));	
	town_offsets[50].push_back (QPoint(523,394));	
	town_offsets[50].push_back (QPoint(464,438));	
	town_offsets[50].push_back (QPoint(394,458));	
	town_offsets[50].push_back (QPoint(322,482));	
	town_offsets[50].push_back (QPoint(213,467));	

	town_offsets[51].push_back (QPoint(193,139));	
	town_offsets[51].push_back (QPoint(266,122));	
	town_offsets[51].push_back (QPoint(373,146));	
	town_offsets[51].push_back (QPoint(445,266));	
	town_offsets[51].push_back (QPoint(423,295));	
	town_offsets[51].push_back (QPoint(264,290));	
	town_offsets[51].push_back (QPoint(291,227));	

	town_offsets[52].push_back (QPoint(159,156));	
	town_offsets[52].push_back (QPoint(257,106));	
	town_offsets[52].push_back (QPoint(736,227));	
	town_offsets[52].push_back (QPoint(662,268));	
	town_offsets[52].push_back (QPoint(545,293));	
	town_offsets[52].push_back (QPoint(475,287));	
	town_offsets[52].push_back (QPoint(357,258));	
	town_offsets[52].push_back (QPoint(374,224));	
	town_offsets[52].push_back (QPoint(300,210));	
	town_offsets[52].push_back (QPoint(204,218));	

	town_offsets[53].push_back (QPoint(61 ,55));	
	town_offsets[53].push_back (QPoint(156,43));	
	town_offsets[53].push_back (QPoint(278,79));	
	town_offsets[53].push_back (QPoint(615,207));	
	town_offsets[53].push_back (QPoint(454,215));	
	town_offsets[53].push_back (QPoint(371,244));	
	town_offsets[53].push_back (QPoint(261,261));	
	town_offsets[53].push_back (QPoint(193,217));	
	town_offsets[53].push_back (QPoint(104,197));	
	town_offsets[53].push_back (QPoint(124,151));	
	town_offsets[53].push_back (QPoint(96,93));	

	town_offsets[54].push_back (QPoint(436,102));	
	town_offsets[54].push_back (QPoint(373,100));	
	town_offsets[54].push_back (QPoint(323,140));	
	town_offsets[54].push_back (QPoint(162,190));	
	town_offsets[54].push_back (QPoint(157,263));	
	town_offsets[54].push_back (QPoint(171,304));	
	town_offsets[54].push_back (QPoint(245,344));	
	town_offsets[54].push_back (QPoint(338,343));	
	town_offsets[54].push_back (QPoint(407,307));	
	town_offsets[54].push_back (QPoint(270,264));	

	town_offsets[55].push_back (QPoint(219,96));	
	town_offsets[55].push_back (QPoint(168,139));	
	town_offsets[55].push_back (QPoint(104,159));	
	town_offsets[55].push_back (QPoint(232,230));	
	town_offsets[55].push_back (QPoint(274,268));	
	town_offsets[55].push_back (QPoint(378,289));	
	town_offsets[55].push_back (QPoint(422,311));	
	town_offsets[55].push_back (QPoint(507,292));	
	town_offsets[55].push_back (QPoint(491,160));	

	town_offsets[56].push_back (QPoint(506,120));	
	town_offsets[56].push_back (QPoint(442,148));	
	town_offsets[56].push_back (QPoint(363,192));	
	town_offsets[56].push_back (QPoint(278,212));	
	town_offsets[56].push_back (QPoint(204,241));	
	town_offsets[56].push_back (QPoint(156,278));	
	town_offsets[56].push_back (QPoint(224,317));	
	town_offsets[56].push_back (QPoint(204,372));	
	town_offsets[56].push_back (QPoint(247,410));	
	town_offsets[56].push_back (QPoint(620,323));	

	town_offsets[57].push_back (QPoint(243,93));	
	town_offsets[57].push_back (QPoint(292,144));	
	town_offsets[57].push_back (QPoint(184,201));	
	town_offsets[57].push_back (QPoint(158,255));	
	town_offsets[57].push_back (QPoint(246,285));	
	town_offsets[57].push_back (QPoint(257,344));	
	town_offsets[57].push_back (QPoint(337,364));	
	town_offsets[57].push_back (QPoint(394,324));	

	town_offsets[58].push_back (QPoint(186,315));	
	town_offsets[58].push_back (QPoint(213,373));	
	town_offsets[58].push_back (QPoint(290,407));	
	town_offsets[58].push_back (QPoint(443,405));	
	town_offsets[58].push_back (QPoint(487,352));	
	town_offsets[58].push_back (QPoint(610,351));	

	town_offsets[59].push_back (QPoint(229,102));	
	town_offsets[59].push_back (QPoint(316,117));	
	town_offsets[59].push_back (QPoint(408,164));	
	town_offsets[59].push_back (QPoint(367,223));	
	town_offsets[59].push_back (QPoint(475,239));	
	town_offsets[59].push_back (QPoint(482,301));	
	town_offsets[59].push_back (QPoint(362,340));	
	town_offsets[59].push_back (QPoint(272,414));	

	town_offsets[60].push_back (QPoint(115,344));	
	town_offsets[60].push_back (QPoint(336,287));	
	town_offsets[60].push_back (QPoint(454,396));	
	town_offsets[60].push_back (QPoint(471,325));	
	town_offsets[60].push_back (QPoint(571,327));	
	town_offsets[60].push_back (QPoint(605,264));	
	town_offsets[60].push_back (QPoint(635,190));	
	town_offsets[60].push_back (QPoint(609,135));	
	town_offsets[60].push_back (QPoint(556,98));	
*/