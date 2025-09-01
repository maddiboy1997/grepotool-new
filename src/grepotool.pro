#
# grepotool.pro - QMake project definition file
#
# Copyright (c) 2013 P. Vorpahl
#
# This file is part of Grepotool, a free utility for the browsergame 
# Grepolis which is the intellectual property of InnoGames GmbH.
# 
# Grepotool is free Software: you can redistribute it and/or modify 
# it under the terms of the GNU Lesser General Public License as 
# published by the Free Software Foundation, either version 3 of the 
# License, or any later version. Alternatively, you may use it under 
# the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or any later 
# version.
# 
# Grepotool is distributed in the hope that it will be useful, but 
# WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License 
# along with 'Grepotool; see the files 'GPL.txt' and 'LGPL.txt' in 
# the base directory of your installation. If not, see 
# 'http://www.gnu.org/licenses/'.

# Replace this with your local installation of Qt

QT_DIR   = "C:/Qt/5.2.0/qtbase"


TARGET   = Grepotool

TEMPLATE = app

RC_FILE = grepotool.rc

RESOURCES = grepotool.qrc

TRANSLATIONS = Grepotool_en.ts \
               Grepotool_de.ts
                
FORMS    = allys.ui \
           completer.ui \
           datetime.ui \
           dlg_about.ui \
           dlg_server.ui \
           lists.ui \
           players.ui \
           specials.ui \
           town_info.ui \
           towns.ui \
           towns2.ui \
           units.ui 
           
SOURCES  = allys.cpp \
           ally_model.cpp \
           app.cpp \
           bashies.cpp \
           command.cpp \
           conquests.cpp \
           data.cpp \
           database.cpp \
           dataloader.cpp \
           defines.cpp \
           edge.cpp \
           explorer.cpp \
           global_data.cpp \         
           history.cpp \
           infmap.cpp \
           islands.cpp \
           item_delegate.cpp \
           json.cpp \
           line_seg.cpp \
           main.cpp \
           mainwin.cpp \
           map.cpp \
           mapview.cpp \
           mesh_thread.cpp \
           players.cpp \
           player_model.cpp \
           popup.cpp \
           selector.cpp \
           server_info.cpp \
           setting.cpp \
           settings.cpp \
           special_selector.cpp \       
           splash.cpp \    
           static_data.cpp \
           tabletool.cpp \
           table_model.cpp \
           table_view.cpp \
           tab_allys.cpp \
           tab_mytowns.cpp \
           tab_plans.cpp \
           tab_players.cpp \
           tab_targets.cpp \
           towns.cpp \
           town_model.cpp \
           town_info.cpp \
           triangle.cpp \
           tri_mesh.cpp \
           toolwidget.cpp \  
           unit_selector.cpp \
           vertex.cpp \
           window.cpp \
           world_data.cpp 
                      
HEADERS  = ally_model.h \ 
           app.h \
           command.h \
           command_ids.h \
           data.h \
           database.h\
           dataloader.h \
           defines.h \
           explorer.h \
           global_data.h \
           indexes.h \
           infmap.h \ 
           json.h \
           mainwin.h \
           map.h \
           mapview.h \
           player_model.h \
           popup.h \
           selector.h \
           server_info.h \
           setting.h \
           special_selector.h \          
           splash.h \
           toolwidget.h \            
           tabletool.h \      
           table_model.h \    
           tab_allys.h \
           tab_mytowns.h \
           tab_plans.h \
           tab_players.h \
           tab_targets.h \
           town_info.h \      
           town_model.h \
           tri_mesh.h \
           unit_selector.h \        
           window.h \  
           world_data.h
           
# Destination for binaries

DESTDIR  = ../bin

# Qt modules to be used

QT      += core gui network widgets

# Tell the compiler that the target uses multithreading, exception handling, 
# OpenGL and the stl

CONFIG	+= stl qt thread exceptions

# A build subdirectory to keep the main source directory clean

BUILD_PATH = ./build

# setup additional include paths and libraries
#LIBS        += -lzlibwapi
#LIBS        += -L./zlib

DEPENDPATH  += .
INCLUDEPATH += .
INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib
# INCLUDEPATH += $${QT_DIR}/include

OTG = $$TARGET

PATH += $${DESTDIR}

# Specific project template for Microsoft compilers 
# All others work fine with Makefiles

win32-msvc*{
    TEMPLATE = $$join(TEMPLATE,,"vc",)
}

# Resource compilation directory 

RCC_DIR	= $${BUILD_PATH}/.rcc

# Some definitions for debug build

build_pass:CONFIG (debug, debug|release) {

	# for debug output, we need the system console	

	CONFIG	+= console
	
	# append a 'd' (for debug) to the target
	
	TARGET	= $$join(OTG,,,d)
	
	# set up object directories to keep the main directory clean
	
	unix:OBJECTS_DIR	= $${BUILD_PATH}/debug/.obj/unix
	win32:OBJECTS_DIR	= $${BUILD_PATH}/debug/.obj/win32
	mac:OBJECTS_DIR	    = $${BUILD_PATH}/debug/.obj/mac
	UI_DIR	= $${BUILD_PATH}/debug/.ui
	MOC_DIR	= $${BUILD_PATH}/debug/.moc

	# turn memory logging on
	DEFINES += __LOG_MEMORY

	# set global debugging define
	DEFINES += __DEBUG_BUILD
}

# Release build 

build_pass:CONFIG (release, debug|release) {

	# set up release build directories for objects to keep
	# the main directory clean	

	unix:OBJECTS_DIR	= $${BUILD_PATH}/release/.obj/unix
	win32:OBJECTS_DIR	= $${BUILD_PATH}/release/.obj/win32
	mac:OBJECTS_DIR	    = $${BUILD_PATH}/release/.obj/mac
	UI_DIR	= $${BUILD_PATH}/release/.ui
	MOC_DIR	= $${BUILD_PATH}/release/.moc
} 








