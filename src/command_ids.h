#ifndef ___COMMAND_IDS_H
#define ___COMMAND_IDS_H

/* command_ids.h  - Menu command ids

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

#define TXT_DEFAULT "Default"
#define TXT_CURRENT_LIST "CurrentList"





// ----------------------------------------------
// Global Settings (stored in Cfg-file)
// ----------------------------------------------
#define WORLDS                     "Global/Servers"
#define CURRENT_WORLD              "Global/CurrentWorld"
#define CURRENT_LANGUAGE           "Global/CurrentLanguage"
#define CURRENT_SERVER_LANG        "Global/CurrentWorldServerLang"
#define GLOBAL_DATADIR                    "Global/Datadir"

#define STORE_SETTINGS_PER_WORLD   "Global/StoreSettingsPerWorld"

// ----------------------------------------------
// Window names
// ----------------------------------------------
#define MAINWINDOW                 "World/MainWin"
#define MAPVIEW                    "World/Mapview"
#define ALLY_TAB                   "World/Allys"
#define PLAYER_TAB                 "World/Players"
#define TOWN_TAB                   "World/Towns"
#define MYTOWN_TAB                 "World/MyTowns"
#define TARGET_TAB                 "World/Targets"
#define PLAN_TAB                   "World/Plans"

// ----------------------------------------------
// Window settings
// ----------------------------------------------
#define WINDOW_STATE               "WindowState"
#define WINDOW_GEOMETRY            "WindowGeometry"

// ----------------------------------------------
// Table settings
// World/MyTowns/CurrentIndex
// World/MyTowns/Tables/1/TableName 
// ----------------------------------------------
#define TABLES                     "Tables"
//  -1 for ghosts and players w/o ally or the
// table number in MyTowns etc.
#define TAB_CURRENT_INDEX          "CurrentIndex"

#define TABLE_SETTINGS             "TableSettings"
#define TAB_NAME                   "TableName"
#define TAB_COLUMN_ORDER           "ColumnSortOrder"
#define TAB_COLUMN_VISIBILITY      "ColumnVisibility"
#define TAB_SORT_COLUMN            "SortColumn"
#define TAB_SORT_ORDER             "SortOrder"
#define TAB_SELECTED_IDS           "SelectedIds"

// ----------------------------------------------
// Settings associated with the Map viewer
// ----------------------------------------------

#define MAPVIEW_GLOBAL_ZOOM        "World/Mapview/Zoom"
#define MAPVIEW_GLOBAL_OFFSET      "World/Mapview/Offset"

#define MAPVIEW_OCEAN_COLOR        "World/Mapview/OceanColor"
#define MAPVIEW_GRID_COLOR         "World/Mapview/GridColor"
#define MAPVIEW_OCEAN_LABEL_COLOR  "World/Mapview/OceanLabelColor"
#define MAPVIEW_SHOW_OCEAN_LABELS  "World/Mapview/ShowOceanLabels"
#define MAPVIEW_SHOW_GRID          "World/Mapview/ShowGrid"
#define MAPVIEW_SHOW_GHOSTS        "World/Mapview/ShowGhosts"
#define MAPVIEW_GHOST_COLOR        "World/Mapview/GhostColor"
#define MAPVIEW_ROCK_ZOOM          "World/Mapview/RockZoom"

#define MAPVIEW_UNSELECTED_MODE    "World/Mapview/UnselectedMode"
#define MAPVIEW_UNSELECTED_COLOR   "World/Mapview/UnselectedColor"

#define MAPVIEW_INFMAP_ZOOM        "World/Mapview/InfMapZoom"
#define MAPVIEW_INFMAP_LABEL_ZOOM  "World/Mapview/InfMapLabelZoom"
#define MAPVIEW_INFMAP_OPACITY     "World/Mapview/InfMapOpacity"
#define MAPVIEW_INFMAP_CITY_REACH  "World/Mapview/InfMapCityReach"

#define MAPVIEW_TOWN_LABEL_ZOOM    "World/Mapview/TownLabelZoom"
#define MAPVIEW_TOWN_LABEL_PLAYER  "World/Mapview/TownLabelPlayer"
#define MAPVIEW_TOWN_LABEL_ALLY    "World/Mapview/TownLabelAlly"
#define MAPVIEW_TOWN_LABEL_POINTS  "World/Mapview/TownLabelPoints"

#define MAPVIEW_TOWN_DBLCLICK      "World/Mapview/TownDblClick"
#define MAPVIEW_MOUSE_WHEEL_INVERT "World/Mapview/MouseWheelInvert"

#define MAPVIEW_GOTO_ZOOM          "World/Mapview/GotoZoom"
#define MAPVIEW_INFMAP_LABEL_MODE  "World/Mapview/InfMapLabelMode"

// commands without settings


// ----------------------------------------------
// Settings associated with the Ally browser
// ----------------------------------------------
#define ALLY_TAB_CURRENT_ALLY       "World/Allys/CurrentAlly"
#define ALLY_TAB_VISIBILITY         "World/Allys/TabVisibility"
#define ALLY_TAB_ORDER              "World/Allys/TabOrder"
#define ALLY_TAB_SORT_COL           "World/Allys/TabSortColumn"
#define ALLY_TAB_SORT_DOWN          "World/Allys/TabSortDown"

// ----------------------------------------------
// Settings associated with the Player browser
// ----------------------------------------------
#define PLAYER_TAB_VISIBILITY       "World/Players/TabVisibility"
#define PLAYER_TAB_ORDER            "World/Players/TabOrder"
#define PLAYER_TAB_SORT_COL         "World/Players/TabSortColumn"
#define PLAYER_TAB_SORT_DOWN        "World/Players/TabSortDown"

// selection
#define PLAYER_TAB_CURRENT_PLAYER   "World/Players/TabCurrentPlayer"
#define PLAYER_TAB_CURRENT_ALLY     "World/Players/TabCurrentAlly"

#define PLAYER_ACTIVITY_CALC        "World/Players/TabActivityCalc"

// commands without settings
#define PLAYER_TAB_SHOW_ALLYLESS    "World/Players/ShowAllyless"

// ----------------------------------------------
// Settings associated with the Town browser
// ----------------------------------------------
#define TOWN_TAB_CURRENT_INDEX       "World/Towns/TabCurrentIndex"
#define TOWN_TAB_CURRENT_TOWN       "World/Towns/TabCurrentTown"
#define TOWN_TAB_CURRENT_TOWN_SEL   "World/Towns/TabCurrentTownSel"


// commands without settings
#define TOWN_TAB_SHOW_GHOSTS        "World/Towns/ShowGhosts"


// ----------------------------------------------
// Settings associated with the MyTowns, Targets 
// and Plan editors
// ----------------------------------------------
// selection
#define MYTOWN_CURRENT_LIST         "World/MyTowns/CurrentList"
#define TARGET_CURRENT_LIST         "World/Targets/CurrentList"
#define PLAN_CURRENT_LIST           "World/Plans/CurrentList"

// The following are always associated with 
// the current list
#define MYTOWN_TAB_VISIBILITY       "World/MyTowns/CurrentList/TabVisibility"
#define MYTOWN_TAB_ORDER            "World/MyTowns/CurrentList/TabOrder"
#define MYTOWN_TAB_SORT_COL         "World/MyTowns/CurrentList/TabSortColumn"
#define MYTOWN_TAB_SORT_DOWN        "World/MyTowns/CurrentList/TabSortDown"
#define MYTOWN_SEL                  "World/MyTowns/CurrentList/MyTownSelectedRow"
#define MYTOWN_SEL_ROWS             "World/MyTowns/CurrentList/MyTownSelectedRows"

#define PLAN_TAB_VISIBILITY       "World/Plans/CurrentList/TabVisibility"
#define PLAN_TAB_ORDER            "World/Plans/CurrentList/TabOrder"
#define PLAN_TAB_SORT_COL         "World/Plans/CurrentList/TabSortColumn"
#define PLAN_TAB_SORT_DOWN        "World/Plans/CurrentList/TabSortDown"
#define PLAN_SEL                  "World/Plans/CurrentList/SelectedRow"
#define PLAN_SEL_ROWS             "World/Plans/CurrentList/SelectedRows"


#define TARGET_TAB_VISIBILITY       "World/Targets/CurrentList/TabVisibility"
#define TARGET_TAB_ORDER            "World/Targets/CurrentList/TabOrder"
#define TARGET_TAB_SORT_COL         "World/Targets/CurrentList/TabSortColumn"
#define TARGET_TAB_SORT_DOWN        "World/Targets/CurrentList/TabSortDown"
#define TARGET_SEL                  "World/Targets/CurrentList/SelectedRow"
#define TARGET_SEL_ROWS             "World/Targets/CurrentList/SelectedRows"


// commands without settings
#define MYTOWN_ADD_TOWN             "MyTownAddTown"
#define MYTOWN_ADD_TOWNS            "MyTownAddTowns"
#define MYTOWN_DEL_SEL              "MyTownDelSel"
#define MYTOWN_DUPLICATE            "MyTownDuplicateSel"
#define MYTOWN_TIME_SETTING         "MyTownTimeSetting"
#define MYTOWN_SEL_UNIT             "MyTownSelectUnit"
#define MYTOWN_TOWN_SPECIALS        "MyTownTownSpecials"
#define MYTOWN_EDIT_LIST_NAME       "MyTownEditListName"
#define MYTOWN_NEW_LIST             "MyTownNewList"
#define MYTOWN_DEL_LIST             "MyTownDelList"
#define MYTOWN_CLEAR_LIST           "MyTownClearList"
#define MYTOWN_EXPORT_LIST          "MyTownExportList"

#define TARGET_ADD_TOWN             "TargetAddTown"
#define TARGET_ADD_TOWNS            "TargetAddTowns"
#define TARGET_DEL_SEL              "TargetDelSel"
#define TARGET_TIME_SETTING         "TargetTimeSetting"
#define TARGET_SEL_UNIT             "TargetUnit"
#define TARGET_ATTACKING_TOWN       "TargetAttTown"
#define TARGET_EDIT_LIST_NAME       "TargetEditListName"
#define TARGET_NEW_LIST             "TargetNewList"
#define TARGET_DEL_LIST             "TargetDelList"
#define TARGET_CLEAR_LIST           "TargetClearList"
#define TARGET_EXPORT_LIST          "TargetExportList"

#define PLAN_ADD_PLANS            "PlanAddPlans"
#define PLAN_DEL_SEL              "PlanDelSel"
#define PLAN_DUPLICATE            "PlanDuplicateSel"
#define PLAN_TIME_SETTING         "PlanAttTimeSetting"
#define PLAN_SEL_UNIT             "PlanSelectAttUnit"
#define PLAN_SEL_TARGET           "PlanSelectTarget"
#define PLAN_SEL_SOURCE           "PlanSelectSource"
#define PLAN_TOWN_SPECIALS        "PlanAttTownSpecials"
#define PLAN_EDIT_LIST_NAME       "PlanEditListName"
#define PLAN_NEW_LIST               "PlanNewList"
#define PLAN_DEL_LIST               "PlanDelList"
#define PLAN_CLEAR_LIST             "PlanClearList"
#define PLAN_EXPORT_LIST            "PlanExportList"



#endif
