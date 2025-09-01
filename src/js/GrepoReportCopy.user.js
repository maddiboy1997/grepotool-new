// ==UserScript==
// @name        GrepoToolSupport
// @description Support for the Grepo-Tool (http://sourceforge.net/projects/grepo-tool/)
// @namespace   de.diabhoil
// @include     http://*.grepolis.com/game*
// @version     0.1 beta
// @grant       GM_xmlhttpRequest
// ==/UserScript==

/*

Copyright (C) 2013 H. Stürmann

This file is a free utility for the browsergame
Grepolis which is the intellectual property of InnoGames GmbH.

GrepoToolSupport is free Software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or any later version. Alternatively, you may use it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or any later
version.

GrepoToolSupport is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

var uw = unsafeWindow || window;
var $ = uw.jQuery;

$("body").ajaxComplete(function () {});

$("body").ajaxSuccess(function () {
	GrepoToolSupportGM.init();
});

var GrepoToolSupportGM = {
	init : function () {
		this.DEBUG = true;

		this.GT_VERSION = "1.1";
        this.GT_ICON = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAGHUlEQVRYw81Xa0xTZxh%2BzqF3WwUtCLVsKI1TAsrm3UUh27xsmsk0XiFKIsuWMS8bRlw0i7c5IzMzUbM455xTMxdnouCmMuaFqaBiJsxhJ4"
                        + "UiVqC0QC%2B0nHN6zvn2o7ZUbulmjPuS8%2BOc9%2Fve6%2FM%2B73eA%2F%2BtavXp18nN1ID4%2BfkNubm7ks7YT0ZdAFMU32tvbE2w2W8WzdIDuT%2Bh2u1c%2B6wz064DVah2fl5f3TLEg6U%2FIcRwuXbqU%2FcrFjyO9AjvPyX"
                        + "u0ds6NSMkAaCTKDpVEVjFYqjlQOn3HiXCM7Th8Uetoa03flbfwpyccMBgMa1mWfQJwTqczDQCam5sX06zTaGFatQGZjXPBxrnUoJBOgUofU7Imc2tS5sIM3USmL%2BMf7Smae%2BHm%2FYM6DS4AeNIBi8WyhmGYhN4ONjU16dNq6POW%"
                        + "2BF6EBCAgqHI%2FmLv13g97AbzbfUtRabXi0M8Vm6%2FfbcjneB7DNIrwMQAAhBBYD17XREWoGHSwQBsDeDmAEL8HFAAKuNfxKCfn1t7U0LPfF11T7ztZWvbA6sjneAEg%2FxKEgWWpb5g13C4vwW0rUNEEVLYA9k6%2FwsdPp8DicnPV"
                        + "%2B916WWttdab%2BZxCGtGNk9GVn7aKx01bpo%2BNMN81%2FTq5ssGx0q6USRNAAywMeH9qZxnkAupygpaDoEKqhKJhb3PWhuikAUCgU5r4wEFiJiYkltbW1MwLvkz5bln2DMx%2BGIAIuDjIWiFZE1m98M2fCB7Mz7QvWfztboVIlV5ut"
                        + "BaCooB61Qno6aqDqTAREfPHe9GNhO6DRaLBp06b4%2FPx8CwDUNTZIZhbklEWpBj2S0RGnV83IKl8yfa4xsH9i5ueXfdIBaSCBMLtRMM%2Fix63zh0vCJQy3243S0tIMAPsAYITuBR7AhID82rY%2BqIDq2TkB4IIKE4QAIJfLkZKSUhI2x"
                        + "YkCIPAIQj%2FQAUQEBB%2BIwANimCAEAJ1OV7Fz505j7PHFGYhUZITWNaRnIWvlv2lYfvyqt70RIiWDfIgekMj8ERMC0esA67SBggCn0xm%2BA7GxsUfMZjOaPe2pUCpXgKZ69rUoQu7hLwO4OkTOnpfKI%2Bx2gV9ASWTBLdEaicnjcV0jA"
                        + "FwuV0dYJVCpVExWVtaJHrXsZ105c2jn8sxF67pjICY6%2BlrZuaPZ5eeOZqenp9vDyoBSqTyVm5trBwAwPODhAIoCaApQSPxIIlSvmHuqcRxY48aNCw4P1LTBz4jNQHUr4BODxsNITO9MqNfrT7Esq%2B02Dce6XK7UmJgYx7Zt284XFxf"
                        + "7Bax4GgJfDwBDI7Vaq0gK%2FAB7ivuAyWRaFwTJ%2FnlnbXFQGzbfv%2BGqcqUaUkZd2c4XpwM4DwD4uuoOgDsA8MmdLQlra44UhGWJhFkCeSciwItpMqVcQdM01Jmp7lvO2oKTD39Xd9%2F7B%2F8wFRK6%2F%2BgJ6SIkioLZ6qrvdxi9K"
                        + "A46Y2lvmg0AcXFxJqPOa2hmHMn51cd%2Bzas8tOWtuPHG31qq9EXWiuwTj66u8CsnfhwQfysGET9YgwiKghhCgQM0A9X9OvDh%2BAUnasq%2B%2BhIigWHqmBvlnCMTBKjzWifvNhed211X6A8nYDQ0UgcD2LouRSP0MQ4pTcCG9AcrkAVvr"
                        + "z9SHz9YOXn%2FhkVZPUqw9LV5jhQ6brdCKuPZJQaeFfiu3iL%2BNAYJPfDdRwCrB0qjm59GJ1oCukYO1zlkNN8RLAOAjk424WGLc299U1tmn224ftLS7eOTXz5W2%2FjgVXg4QBBDuPxxNIQAvAh0cIDZAW0Ny7wzZMLSkk8PPzEvZGLnXcJ6"
                        + "%2FftJl5KOTh9MJpO2VyKa9dpMprqmprz8YB5jb7CBxKgAjQyQ0f4MiARgBcDFgrJ5kUTHmeYkTlm6K3djj5%2BYaDV94GFN3WT5wKGgpXIAFIjgQ5vPi%2Fsmc2y%2FxPXLxWLFd2WFG%2F5qM6%2B0MK16D%2BHAQ4AMEkTRKuiUg42joxIO"
                        + "rZw2f9%2Fr09L6vBHPWbZqj6XFucbt5QBQkEkpDIsexMycmjQlbOY8VXh6VFWDMbbe1ogJI1KQoNUZ586Z0xzu%2BcKzZ%2FW3K%2F82%2BHgfxo0Z5Rj90khjUlIS89z%2Fwv8Bj%2FeoZpa%2BSGIAAAAASUVORK5CYII%3D";
		this.player_server = uw.Game.world_id;
		this.player_language = uw.Game.locale_lang;
		this.player_name = uw.Game.player_name;
		this.UNIT_TYPES = new Array("militia", "sword", "slinger", "archer", "hoplite", "rider", "chariot", "catapult", "minotaur", "manticore", "zyklop", "harpy", "medusa", "centaur", "pegasus", "cerberus", "fury", "griffin", "calydonian_boar", "godsent", "big_transporter", "bireme", "attack_ship", "demolition_ship", "small_transporter", "trireme", "colonize_ship", "sea_monster");
        var that = this;
		if ($("#report_game_body").length > 0) { // unter Berichte
			if (this.check2("#report_game_body")) {
				//if (this.checkIfEnemy("#report_game_body")) {
					$("#report_report div.game_list_footer").ready(function () {
						if ($("#report_date").length > 0)
							if (($("#report_report #Copy_bericht").length) == 0)
                                that.createButtonForPrivateReport("report");
					});
				//}
			}
		}

		if ($(".published_report").length > 0) { //veröffentlichte Reports
			
			$("#postlist").ready(function () { //Forum
				$("#postlist").find(".published_report").each(function () {
					var reportID = $(this).attr("id");
					that.log("reportID: " + reportID);
					if ($("#postlist .published_report " + "#Copy_" + reportID).length == 0)
						that.createButtonForPublicReport(reportID, "forum");
				});
			});

			$("#memo_preview_text").ready(function () { //Notizen
				$("#memo_preview_text").find(".published_report").each(function () {
					var reportID = $(this).attr("id");
					that.log("reportID: " + reportID);
					if ($("#memo_preview_text .published_report " + "#Copy_" + reportID).length == 0)
						that.createButtonForPublicReport(reportID, "note");
				});
			});

			$("#message_message_list").ready(function () { //Nachrichten

				$("#message_message_list").find(".published_report").each(function () {
					var reportID = $(this).attr("id");
					that.log("reportID: " + reportID);
					if ($("#message_message_list .published_report " + "#Copy_" + reportID).length == 0)
						that.createButtonForPublicReport(reportID, "pn");
				});
			});

		}
	},
    insertafter : function (a, toInsert) {
		return a.parentNode.insertBefore(toInsert, a.nextSibling);
	},
    createButtonForPrivateReport : function (source) {
    
            var that = this;
        
        var copyFunc = function (event) {
            var attacker_name = "";
			var attacker_town = "";
			var defender_name = "";
			var defender_town = "";
                
            attacker_town = $("#report_header #report_sending_town .gp_town_link").attr("href").split("#");
            attacker_town = JSON.parse(atob(attacker_town[attacker_town.length - 1])).id;
            defender_town = $("#report_header #report_receiving_town .gp_town_link").attr("href").split("#");
            defender_town = JSON.parse(atob(defender_town[defender_town.length - 1])).id;
            

            attacker_name = $("#report_header #report_sending_town .gp_player_link").attr("href").split("#");
            attacker_name = JSON.parse(atob(attacker_name[attacker_name.length - 1])).id;
            
            defender_name = $("#report_header #report_receiving_town .gp_player_link").attr("href").split("#");
            defender_name = JSON.parse(atob(defender_name[defender_name.length - 1])).id;

            
            var attack_date = $("#report_date").text();
			attack_date = $.trim(attack_date);
			
            var resultArray = {};
			resultArray.attacker_units = {};
            resultArray.defender_units = {};
			resultArray.attacker_name = attacker_name;
			resultArray.attacker_town = attacker_town;
            resultArray.defender_name = defender_name;
            resultArray.defender_town = defender_town;
			resultArray.attack_date = attack_date;
            resultArray.source = source;
			resultArray = that.collectDataFromReport(resultArray);

			var resultString = that.getResultString(resultArray);

			that.showResultString(event, resultString);
        };
        
        var doc = document;
        
        var buttonDiv = doc.createElement("div");
		buttonDiv.id = "Copy_bericht";
		buttonDiv.style.cssText = "float:right; height: 22px; width: 22px; padding:2px;";

		var button = doc.createElement('a');
		button.style.cssText = "float:left; display:inline-block;";
		button.href = "#";
		button.onclick = copyFunc;

		var button_span = doc.createElement('img');
		button_span.style.cssText = "width:22px;height:22px;display:block;";
		button_span.id = "gts_gm_report";
        button_span.src = this.GT_ICON;
		button.appendChild(button_span);

		buttonDiv.appendChild(button);
		that.insertafter(doc.getElementById("report_date"), buttonDiv);
        
    },
    createButtonForPublicReport : function (reportID, source) {
        
        var that = this;
        
        var copyFunc = function (event) {
        
            var attacker_name = "";
			var attacker_town = "";
			var defender_name = "";
			var defender_town = "";
            
            var town_names = $("#" + reportID + " .published_report_header .gp_town_link");

            if(town_names.length != 2)
                return;
                
            attacker_town = town_names.first().attr("href").split("#");
            attacker_town = JSON.parse(atob(attacker_town[attacker_town.length - 1])).id;
            defender_town = town_names.last().attr("href").split("#");
            defender_town = JSON.parse(atob(defender_town[defender_town.length - 1])).id;
            
            if(town_names.first().next().attr("class") == "gp_player_link")
            { // Angreifer bekannt
                attacker_name = town_names.first().next().attr("href").split("#");
                attacker_name = JSON.parse(atob(attacker_name[attacker_name.length - 1])).id;
            
            }
            else
            { // Verteidiger bekannt
                defender_name = $("#" + reportID + " .published_report_header .gp_player_link").attr("href").split("#");
                defender_name = JSON.parse(atob(defender_name[defender_name.length - 1])).id;
            }
            
            var attack_date = $("#" + reportID + " .published_report_header .reports_date").text();
			attack_date = $.trim(attack_date);
			
            var resultArray = {};
			resultArray.attacker_units = {};
            resultArray.defender_units = {};
			resultArray.attacker_name = attacker_name;
			resultArray.attacker_town = attacker_town;
            resultArray.defender_name = defender_name;
            resultArray.defender_town = defender_town;
			resultArray.attack_date = attack_date;
            resultArray.source = source;
			resultArray = that.collectDataFromPublishedReport(resultArray, reportID);

			var resultString = that.getResultString(resultArray);

			that.showResultString(event, resultString);
        };
        
        var doc = document;
        
        var tmp = document.getElementById(reportID);
        if(!tmp)
            return;
        var aim = tmp.getElementsByClassName("surv")[0];
        
        var buttonDiv = doc.createElement("div");
		buttonDiv.id = "Copy_" + reportID;
		buttonDiv.style.cssText = "float:right; height: 22px; width: 22px; padding:2px;";

		var button = doc.createElement('a');
		button.style.cssText = "float:left; display:inline-block;";
		button.href = "#";
		button.onclick = copyFunc;

		var button_span = doc.createElement('img');
		button_span.style.cssText = "width:22px;height:22px;display:block;";
        button_span.src = this.GT_ICON;
		button.appendChild(button_span);

		buttonDiv.appendChild(button);
		that.insertafter(aim, buttonDiv);
    },
	log : function (text) {
		if (this.DEBUG)
			console.debug(text);
	},
	showResultString : function (e, resultString) {
        var that = this;
        var height = 180;
        var width = 350;
        var left = e.clientX-width;
        var top = e.clientY-height;
        
        //ganz außen
        var gt_popup_holder = document.createElement("div");
        gt_popup_holder.id = "gts_copy_wnd_123";
        gt_popup_holder.style.cssText = "width:100%;height:100%;z-index:7000;text-align:center;position:absolute;";
        
        //mittig zum Anzeigen des Inhaltes
        var gt_popup_inner = document.createElement("div");
        gt_popup_inner.style.cssText = "width:350px;height:180px;margin:auto;position:absolute;top:0px;bottom:0px;left:0px;right:0px;z-index:7001;";
        
        
        //Kopf-Element mit Close Button
        var gt_headerDiv = document.createElement("div");
        gt_headerDiv.style.cssText = "height:20px;text-align:left;background-color:#FFE2A0";
        
        var header_title_div = document.createElement("div");
        header_title_div.style.cssText = "font-weight:bold;padding-top:3px;padding-left:5px;float:left;";
        header_title_div.textContent = "Copy with CTRL-C";
        
        
        var close_button_holder = document.createElement("div");
        close_button_holder.style.cssText = "padding-top:3px;float:right;";
        
        var closeButton = document.createElement("a");
        closeButton.href = "#";
        closeButton.style.cssText = "text-decoration:none;color:black;padding:5px;";
        closeButton.textContent = "X";
        closeButton.onclick = function (event) {            
            document.body.removeChild(gt_popup_holder);
        };
        
        close_button_holder.appendChild(closeButton);
        
        var header_clear_div = document.createElement("div");
        header_clear_div.style.cssText = "clear:both";
        
        gt_headerDiv.appendChild(header_clear_div);
        gt_headerDiv.appendChild(header_title_div);
        gt_headerDiv.appendChild(close_button_holder);
        gt_popup_inner.appendChild(gt_headerDiv);
        
        //Div was die Textbox hällt
        var gt_innerDiv = document.createElement("div");
        gt_innerDiv.style.cssText = "background-color:#FFE2A0";
        
        var form = document.createElement("form");
        var textinput = document.createElement("textarea");
        textinput.value = resultString;
        textinput.style.cssText = "height:150px;width:340px;";
        textinput.id = "gts_gm_textinput";
        /*
        textinput.addEventListener("keypress", function press(pressEvent) {
            var c =  String.fromCharCode(pressEvent.which).toUpperCase();
            if (c == "C" && pressEvent.ctrlKey)
            {
                setTimeout(function () { document.body.removeChild(gt_popup_holder);}, 100); 
                
            }
        },false); */
        
       
        
        form.appendChild(textinput);
        gt_innerDiv.appendChild(form);
        
        gt_popup_inner.appendChild(gt_innerDiv);  
        gt_popup_holder.appendChild(gt_popup_inner);
        
        
        //Overlay für den schwarzen Hintergrund
        var popup_overlay = document.createElement("div");
        popup_overlay.style.cssText = "width:100%;height:100%;opacity:0.8;background-color:black";
        
        //Alles anhängen
        gt_popup_holder.appendChild(popup_overlay);
        document.body.appendChild(gt_popup_holder);
        
        textinput.focus();
        textinput.select();
         $("#gts_gm_textinput").bind('keydown', function(event) {
    if (event.ctrlKey || event.metaKey) {
        switch (String.fromCharCode(event.which).toLowerCase()) {
        case 'c':
            setTimeout(function () { document.body.removeChild(gt_popup_holder);}, 100); 
            break;
        default :
        break;
        }
    }
});
        
	},
	getResultString : function (resultArray) {
    
        var resultString = "[GrepoTool=" + this.GT_VERSION + ";";
        resultString += "server=" + this.player_server + ";";
		resultString += "language=" + this.player_language + ";";
		resultString += "date=" + resultArray.attack_date + ";";
		resultString += "source=" + resultArray.source + ";";
		resultString += "attacker_name=" + resultArray.attacker_name + ";";
		resultString += "attacker_town=" + resultArray.attacker_town + ";";
		resultString += "attacker_units=(";
        $.each(resultArray.attacker_units, function (index, value) {
			resultString += value["unit_name"] + ":" + value["count"] + ";";
		});
		resultString += ");";
		resultString += "defender_name=" + resultArray.defender_name + ";";
		resultString += "defender_town=" + resultArray.defender_town + ";";
		resultString += "defender_units=(";
        $.each(resultArray.defender_units, function (index, value) {
			resultString += value["unit_name"] + ":" + value["count"] + ";";
		});
		resultString += ")]";
		return resultString;
	},
	collectDataFromReport : function (resultArray) {

		var that = this;
		$("#report_report .report_units .report_units_type .report_side_attacker_unit").each(function (index) {
			var unit_name_a = $(this).find("div").attr("class");
			unit_name_a = $.trim(unit_name_a).split(" ");
			var tmp = 1;
			var unit_name = unit_name_a[unit_name_a.length - tmp];
			while ($.inArray(unit_name, that.UNIT_TYPES) === -1 && tmp <= unit_name_a.length)
                    unit_name = unit_name_a[unit_name_a.length - tmp++];
            if($.inArray(unit_name, that.UNIT_TYPES) !== -1)
            {
            
			var unit_count = $(this).find("div span").text();
			var unit_lost = $(this).find(".report_losts").text();

			resultArray.attacker_units[index] = {};
			resultArray.attacker_units[index]["unit_name"] = unit_name;
			resultArray.attacker_units[index]["count"] = unit_count;
			resultArray.attacker_units[index]["lost"] = unit_lost;
            }
		});
        
        $("#report_report .report_units .report_units_type .report_side_defender_unit").each(function (index) {
			var unit_name_d = $(this).find("div").attr("class");
			unit_name_d = $.trim(unit_name_d).split(" ");
			var tmp = 1;
			var unit_name = unit_name_d[unit_name_d.length - tmp];
			while ($.inArray(unit_name, that.UNIT_TYPES) === -1 && tmp <= unit_name_d.length)
				unit_name = unit_name_d[unit_name_d.length - tmp++];
            if($.inArray(unit_name, that.UNIT_TYPES) !== -1)
            {
			var unit_count = $(this).find("div span").text();
			var unit_lost = $(this).find(".report_losts").text();

			resultArray.defender_units[index] = {};
			resultArray.defender_units[index]["unit_name"] = unit_name;
			resultArray.defender_units[index]["count"] = unit_count;
			resultArray.defender_units[index]["lost"] = unit_lost;
            }
		});

		return resultArray;
	},

	collectDataFromPublishedReport : function (resultArray, reportID) {
		var that = this;
		$("#" + reportID + " .report_units .report_units_type .report_side_attacker_unit").each(function (index) {
			var unit_name_a = $(this).find("div").attr("class");
			unit_name_a = $.trim(unit_name_a).split(" ");
			var tmp = 1;
			var unit_name = unit_name_a[unit_name_a.length - tmp];
			while ($.inArray(unit_name, that.UNIT_TYPES) === -1 && tmp <= unit_name_a.length)
				unit_name = unit_name_a[unit_name_a.length - tmp++];
            if($.inArray(unit_name, that.UNIT_TYPES) !== -1)
            {
			var unit_count = $(this).find("div span").first().text();
			var unit_lost = $(this).find(".report_losts").text();

			resultArray.attacker_units[index] = {};
			resultArray.attacker_units[index]["unit_name"] = unit_name;
			resultArray.attacker_units[index]["count"] = unit_count;
			resultArray.attacker_units[index]["lost"] = unit_lost;
            }
		});
        
        $("#" + reportID + " .report_units .report_units_type .report_side_defender_unit").each(function (index) {
			var unit_name_d = $(this).find("div").attr("class");
			unit_name_d = $.trim(unit_name_d).split(" ");
			var tmp = 1;
			var unit_name = unit_name_d[unit_name_d.length - tmp];
			while ($.inArray(unit_name, that.UNIT_TYPES) === -1 && tmp <= unit_name_d.length)
				unit_name = unit_name_d[unit_name_d.length - tmp++];
            if($.inArray(unit_name, that.UNIT_TYPES) !== -1)
            {
			var unit_count = $(this).find("div span").first().text();
			var unit_lost = $(this).find(".report_losts").text();

			resultArray.defender_units[index] = {};
			resultArray.defender_units[index]["unit_name"] = unit_name;
			resultArray.defender_units[index]["count"] = unit_count;
			resultArray.defender_units[index]["lost"] = unit_lost;
            }
		});

		return resultArray;
	},
	checkIfEnemy : function (a) {
		var sendTownOwner = $(a + " #report_sending_town .town_owner a.gp_player_link").html();
		if (sendTownOwner == this.player_name)
			return false;
		else
			return true;

	},
	check2 : function (a) {
		if ($(a + " .fight_report_classic").length == 0)
			return false;
		if ($(a + " .report_side_attacker_empty").length > 0)
			return false;
		if ($(a + " .island_quests").length > 0)
			return false;
		if ($(a + " #report_header").length == 0)
			return false;
		return true;
	}
};