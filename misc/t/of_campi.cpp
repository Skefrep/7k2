/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Filename   : OF_CAMPI.CPP
//Description: Interface for Camp

#include <OVGA.H>
#include <OF_CAMP.H> 
#include <ALL.H>
#include <ORACERES.H>
#include <OMONSRES.H>
#include <OHERORES.H>
#include <OSYS.H>
#include <OMODEID.H>
#include <OFONT.H>
#include <OMOUSE.H>
#include <OIMGRES.H>
#include <OREMOTE.H>
#include <OSE.H> 
#include <OHELP.H>
#include <OTRANSL.H>
#include <OBUTT3D.H>
#include <OCONFIG.H>
#include <ONATION.H>
#include <OUNIT.H>
#include <OSPY.H>
#include <OITEMRES.H>
#include <OBITMAP.H>
#include <OSERES.H>

enum	{PATROL_ALL=0,
		 PATROL_NO_GENERAL=1,
		 PATROL_NO_INJURED_SOILDER=2};

//----------- Define static vars -------------//

static patrol_state = PATROL_ALL;
static char	last_menu_mode;
static Button3D button_patrol, button_reward, button_defense, button_camp_upgrade;
static Button3D button_promote;
static short  	 pop_disp_y1;
static int      disp_soldier_list_y1;
static int      disp_soldier_info_y1;
// ###### begin Gilbert 21/9 ######//
static char		 disp_combat_or_skill;	// 0=normal, 1=combat, 2=skill, 3=spying, 4=loyalty
// ###### end Gilbert 21/9 ######//

//--------- Declare static functions ---------//

static void disp_debug_info(FirmCamp* firmPtr, int refreshFlag);
static void disp_soldier_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints);
static void disp_training_bar(int x1, int y1, int x2, int hitPoints, int maxHitPoints);

#define SOLDIER_PER_ROW      8
#define SOLDIER_X_SPACING   24
#define SOLDIER_Y_SPACING   86


//--------- Begin of function FirmCamp::put_info ---------//
//
void FirmCamp::put_info(int refreshFlag)
{
	// ##### begin Gilbert 21/9 ######//
	if( refreshFlag == INFO_REPAINT )
	{
		last_menu_mode = firm_menu_mode = FIRM_MENU_MAIN;
		disp_combat_or_skill = 0;		// display combat
	}
	// ##### end Gilbert 21/9 ######//
	else
	{
		if( last_menu_mode != firm_menu_mode )		// if changing menu mode pass repaint to sub-menu
		{
			refreshFlag = INFO_REPAINT;
			last_menu_mode = firm_menu_mode;
		}
	}

	Firm::put_info(refreshFlag);

	switch( firm_menu_mode )
	{
	case FIRM_MENU_MAIN:
		if( should_show_info() )
		{
			vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );

			disp_camp_info(INFO_Y1, refreshFlag);
			Firm *firmPtr = firm_array[firm_recno];

			if( firm_id == FIRM_CAMP )
				disp_camp_upgrade(INFO_Y1, refreshFlag);

			disp_soldier_list(INFO_Y1, refreshFlag, 0);
			disp_soldier_info(INFO_Y1+178, refreshFlag);
			// disp_debug_info(this, refreshFlag);
		}
		break;

	case FIRM_MENU_TRAIN:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		disp_train_menu(refreshFlag);
		break;

	case FIRM_MENU_SPY:
	case FIRM_MENU_SELECT_BRIBER:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		disp_spy_menu(refreshFlag);
		break;

	case FIRM_MENU_SET_BRIBE_AMOUNT:
//		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_bribe_menu(refreshFlag);
		break;

	case FIRM_MENU_BRIBE_RESULT:
//		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("FORTBASE") );
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_bribe_result(refreshFlag);
		break;

	case FIRM_MENU_VIEW_SECRET:
		spy_array.disp_view_secret_menu(action_spy_recno, refreshFlag);
		break;

	case FIRM_MENU_ASSASSINATE_CONFIRM:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_assassinate_confirm(refreshFlag);
		break;

	case FIRM_MENU_ASSASSINATE_RESULT:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_assassinate_result(refreshFlag);
		break;

	case FIRM_MENU_STEAL_TECH_CONFIRM:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_steal_tech_confirm(refreshFlag);
		break;

	case FIRM_MENU_STEAL_TECH_RESULT:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_steal_tech_result(refreshFlag);
		break;

	case FIRM_MENU_INCIDENT_NATION:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_incident_nation(refreshFlag);
		break;

	case FIRM_MENU_INCIDENT_CONFIRM:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_incident_confirm(refreshFlag);
		break;

	case FIRM_MENU_INCIDENT_RESULT:
		vga.active_buf->put_bitmap( INFO_X1, INFO_Y1, image_gameif.read("BLDGBASE") );
		disp_incident_result(refreshFlag);
		break;

	default:
		err_when( firm_menu_mode < FIRM_MENU_CAMP_LAST );
		// if firm_menu_mode >= FIRM_MENU_CAMP_LAST, handled by sub-class
	}
}
//----------- End of function FirmCamp::put_info -----------//


//--------- Begin of function FirmCamp::detect_info ---------//
//
void FirmCamp::detect_info()
{
	Firm::detect_info();

	switch( firm_menu_mode )
	{
	case FIRM_MENU_MAIN:
		if( should_show_info() )
		{
			detect_camp_info();
			detect_soldier_list(0);
			detect_soldier_info();
			detect_spy_button();
		}
		break;

	case FIRM_MENU_TRAIN:
		detect_train_menu();
		break;

	case FIRM_MENU_SPY:
	case FIRM_MENU_SELECT_BRIBER:
		detect_spy_menu();
		break;

	case FIRM_MENU_SET_BRIBE_AMOUNT:
		detect_bribe_menu();
		break;

	case FIRM_MENU_BRIBE_RESULT:
		detect_bribe_result();
		break;

	case FIRM_MENU_VIEW_SECRET:
		if( spy_array.detect_view_secret_menu(action_spy_recno, nation_recno) )
		{
			firm_menu_mode = FIRM_MENU_MAIN;
//			info.disp();
		}
		break;

	case FIRM_MENU_ASSASSINATE_CONFIRM:
		detect_assassinate_confirm();
		break;

	case FIRM_MENU_ASSASSINATE_RESULT:
		detect_assassinate_result();
		break;

	case FIRM_MENU_STEAL_TECH_CONFIRM:
		detect_steal_tech_confirm();
		break;

	case FIRM_MENU_STEAL_TECH_RESULT:
		detect_steal_tech_result();
		break;

	case FIRM_MENU_INCIDENT_NATION:
		detect_incident_nation();
		break;

	case FIRM_MENU_INCIDENT_CONFIRM:
		detect_incident_confirm();
		break;

	case FIRM_MENU_INCIDENT_RESULT:
		detect_incident_result();
		break;

	default:
		err_when( firm_menu_mode < FIRM_MENU_CAMP_LAST );
		// if firm_menu_mode >= FIRM_MENU_CAMP_LAST, handled by sub-class
	}
}
//----------- End of function FirmCamp::detect_info -----------//


//--------- Begin of function FirmCamp::disp_camp_upgrade ---------//
//
void FirmCamp::disp_camp_upgrade(int dispY1, int refreshFlag)
{
	if( is_own() )
	{
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;
		char iconName[]="CTOF-00";
	
		if( refreshFlag==INFO_REPAINT )
		{
			button_camp_upgrade.create( x2 + 162, y2, 'A', iconName );
			button_camp_upgrade.set_help_code( "CAMPTOFORT" );
		}
		button_camp_upgrade.enable_flag = 1;

		if ( upgrading_firm_id )
		{
			iconName[5] = '0' + upgrade_completion_percent/40;
			iconName[6] = '0' + upgrade_completion_percent/4 % 10;
			vga.active_buf->put_bitmap( x2 +162, y2, image_spict.read(iconName) );
		}
		else
			button_camp_upgrade.paint();
	}
}
//----------- End of function FirmCamp::disp_camp_upgrade -----------//

//--------- Begin of function FirmCamp::disp_camp_info ---------//
//
void FirmCamp::disp_camp_info(int dispY1, int refreshFlag)
{
	if( is_own() )
	{
		int x1 = INFO_X1 +13;
		int y1 = INFO_Y1 +235;
		int x2 = INFO_X1 +13;
		int y2 = INFO_Y1 +281;
		if( refreshFlag==INFO_REPAINT )
		{
			// ##### begin Gilbert 31/12 #######//
			// button_patrol.create( INFO_X1+13, INFO_Y1+235, 'A', "PATROL" );
			// button_reward.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+235, 'A', "REWARDCB" );
			// button_defense.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+235, 'A', defense_flag ? "DEFENSE1" : "DEFENSE0" );

			if (!is_monster())
				button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "PATROL" );
			else
				button_patrol.create( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, 'A', "F_PATROL" );

			if (!is_monster())
				button_reward.create( INFO_X1+13, INFO_Y1+235, 'A', "REWARD" );				
			else
				button_reward.create( INFO_X1+13, INFO_Y1+235, 'A', "F_REWARD" );				
							
			if (!is_monster())
				button_defense.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+281, 'A', defense_flag ? "DEFENSE1" : "DEFENSE0" );
			else
				button_defense.create( INFO_X1+13+2*BUTTON_DISTANCE, INFO_Y1+281, 'A', defense_flag ? "F_DEFEN1" : "F_DEFEN0" );
				
			if (!is_monster())
				button_promote.create( INFO_X1+13+2*BUTTON_DISTANCE , INFO_Y1+235, 'A', "PROMOTE" );
			else
				button_promote.create( INFO_X1+13+2*BUTTON_DISTANCE , INFO_Y1+235, 'A', "F_PROMOT" );
			// ##### end Gilbert 31/12 #######//
		}
		if( overseer_recno )
		{
			button_patrol.enable_flag = 1;
		}
		else
		{
			for( int i = 0; i < soldier_count && soldier_array[i].is_under_training(); ++i );
			button_patrol.enable_flag = i < soldier_count;
		}
		
		String str;
		switch(patrol_state)
		{
			case PATROL_ALL:
			//	str = "Sortie All";
				str = "";
				break;
			case PATROL_NO_GENERAL:
				str = "Sortie No Leader";
				break;
			case PATROL_NO_INJURED_SOILDER:
				str = "Sortie No Injured";
				break;
			default:
				break;
		}

		button_patrol.paint();
	//	vga.active_buf->bar_alpha( button_patrol.x1, button_patrol.y1+15, button_patrol.x1+BUTTON_ACTION_WIDTH-1, button_patrol.y1+BUTTON_ACTION_HEIGHT-16, 1, 0 );
		font_whbl.center_put_paragraph( button_patrol.x1, button_patrol.y1, button_patrol.x1+BUTTON_ACTION_WIDTH-1, button_patrol.y1+BUTTON_ACTION_HEIGHT-1, str );

		// ###### begin Gilbert 15/4 ########//
		if( nation_array[nation_recno]->cash >= REWARD_COST &&
			 ( (overseer_recno && unit_array[overseer_recno]->rank_id != RANK_KING && unit_res[unit_array[overseer_recno]->unit_id]->class_info.has_loyalty)
			  || (selected_soldier_id && selected_soldier_id <= soldier_count && unit_res[soldier_array[selected_soldier_id-1].unit_id]->class_info.has_loyalty)) )
			button_reward.enable_flag = 1;			// call paint
		// ###### end Gilbert 15/4 ########//
		else
			button_reward.enable_flag = 0;		// call paint
		button_reward.paint();

		if (!is_monster())
			button_defense.update_bitmap( defense_flag ? "DEFENSE1" : "DEFENSE0" );		// call paint
		else
			button_defense.update_bitmap( defense_flag ? "F_DEFEN1" : "F_DEFEN0" );

		if( (button_promote.visible_flag = !overseer_recno) )
		{
			button_promote.enable_flag = 0;
			if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
			{
				Soldier *soldierPtr = soldier_array + selected_soldier_id - 1;
				// ##### begin Gilbert 24/3 ######//
				err_when( soldierPtr->unit_id == UNIT_WAGON );

				if( soldierPtr->race_id != 0
					&& soldierPtr->rank_id == RANK_SOLDIER 
					&& !soldierPtr->is_under_training()
					&& soldierPtr->skill_level() > 0 )
				// ##### end Gilbert 24/3 ######//
				{
					button_promote.enable_flag = 1;
				}
			}

			button_promote.paint();
		}
	}
	disp_spy_button( INFO_X1+13+BUTTON_DISTANCE, INFO_Y1+281, refreshFlag);
}
//----------- End of function FirmCamp::disp_camp_info -----------//


//--------- Begin of function FirmCamp::detect_camp_info ---------//
//
void FirmCamp::detect_camp_info()
{
	int rc;
	if( is_own() )
	{
		//----------- detect patrol -----------//

		if( (rc = button_patrol.detect(0, 0, 1)) )
		{
			if (rc == 2)
			{
				patrol_state = (++patrol_state) % 3;
			}
			else
			if(remote.is_enable())
			{
				// packet structure : <firm recno> <patrol_state>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_CAMP_PATROL, 2 * sizeof(short));
				shortPtr[0] = firm_recno;
				shortPtr[1] = patrol_state;
			}
			else
			{
				patrol(patrol_state);
			}
		//	se_ctrl.immediate_sound("SORTIE");
			if( overseer_recno )
			{
				Unit *overseer = unit_array[overseer_recno];
				se_res.sound( center_x, center_y, 1, 'S', overseer->sprite_id, "ACK");
			}
			else
			if( soldier_count > 0 )
			{
				Soldier *soldierPtr = &soldier_array[0];
				int unitId = soldierPtr->unit_id;
				UnitInfo *unitInfo = unit_res[unitId];
				se_res.sound( center_x, center_y, 1, 'S', unitInfo->sprite_id, "ACK");
			}				
		}

		//----------- detect reward -----------//

		if( button_reward.detect() )
		{
			reward(selected_soldier_id, COMMAND_PLAYER);
			se_ctrl.immediate_sound("TURN_ON");
		}

		if( firm_id==FIRM_CAMP && button_camp_upgrade.detect() )
		{
			if (!upgrading_firm_id)
			{
				se_ctrl.immediate_sound("TURN_ON");
				start_upgrade(FIRM_FORT, COMMAND_PLAYER);	
			}
		}

		//----- detect defense mode button -------//

		if( button_defense.detect() )
		{
			se_ctrl.immediate_sound( !defense_flag?"TURN_ON":"TURN_OFF");

			if( !remote.is_enable() )
			{
				// update RemoteMsg::toggle_camp_patrol()
				defense_flag = !defense_flag;
			}
			else
			{
				// packet structure : <firm recno> <defense_flag>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_F_CAMP_TOGGLE_PATROL, 2*sizeof(short));
				shortPtr[0] = firm_recno;
				shortPtr[1] = !defense_flag;
			}

//			button_defense.update_bitmap( defense_flag ? "DEFENSE1" : "DEFENSE0" );
		}

		if( button_promote.detect() )
		{
			promote_soldier( selected_soldier_id, COMMAND_PLAYER );
			se_ctrl.immediate_sound("PROMOTE");
		}
	}
}
//----------- End of function FirmCamp::detect_camp_info -----------//



//--------- Begin of function FirmCamp::disp_soldier_list ---------//
//
void FirmCamp::disp_soldier_list(int dispY1, int refreshFlag, int dispSpyMenu)
{
	disp_soldier_list_y1 = dispY1;

	for( int inc = -1; inc <= 1; inc += 2 )
	{
		err_when( inc == 0 );

		// first round is descending draw to icon
		// second round is ascending to draw the frame

		int inAreaFlag = 4;

		for( int i = inc>=0?0:soldier_count; i >= 0 && i <= soldier_count; i +=inc )
		{
			// display soldier i

			int row = i/SOLDIER_PER_ROW;
			int x = INFO_X1 + 18 + (i % SOLDIER_PER_ROW) * SOLDIER_X_SPACING;
			int y = INFO_Y1 + 50 + row * SOLDIER_Y_SPACING;
			int yHp = INFO_Y1 + 7 + row * SOLDIER_Y_SPACING;

			int windowX1 = INFO_X1 + 16;
			int windowX2 = INFO_X1 + 220;
			int windowY1 = INFO_Y1 + 5 + row * 84;	// 5,89
			int windowY2 = windowY1 + 80 - 1 ;

			int unitId;
			int hp;
			int maxHp;
			// ##### begin Gilbert 21/9 ######//
			int combatLevel;
			int skillLevel;
			int loyalty;
			// ##### end Gilbert 21/9 ######//
			int ownSpy;
			int itemId;

			if( i==0 )
			{
				if( !overseer_recno )
					continue;

				// overseer
				Unit *overseer = unit_array[overseer_recno];
				unitId = overseer->unit_id;

				hp = (int) overseer->hit_points;
				maxHp = overseer->max_hit_points();
				combatLevel = (int) overseer->combat_level();
				skillLevel = (int) overseer->skill_level();

				if( overseer->rank_id != RANK_GENERAL )
					loyalty = -1;		// king or other(?)
				else
					loyalty = overseer->loyalty;

				ownSpy = overseer->is_own_spy() ? overseer->spy_recno : 0;
				itemId = overseer->item.id;
			}
			else
			{
				// soldier
				Soldier *soldierPtr = &soldier_array[i-1];
				unitId = soldierPtr->unit_id;

				hp = soldierPtr->hit_points;
				maxHp = soldierPtr->max_hit_points();
				combatLevel = (int) soldierPtr->combat_level();
				skillLevel = (int) soldierPtr->skill_level();

				// ####### begin Gilbert 24/3 #########//
				if( unit_res[soldierPtr->unit_id]->class_info.has_loyalty && nation_recno )
				// ####### end Gilbert 24/3 #########//
					loyalty = soldierPtr->loyalty;
				else
					loyalty = -1;
				ownSpy = soldierPtr->is_own_spy() ? soldierPtr->spy_recno : 0;
				itemId = soldierPtr->item.id;
			}

			if( dispSpyMenu && !ownSpy )			// skip displaying spy
				continue;

			UnitInfo *unitInfo = unit_res[unitId];

			// display that solider icon at x+SOLDIER_X_SPACING/2, y
			// draw a frame if selected

			if( inc < 0 )
			{
				// first round is descending draw to icon
				Soldier *soldierPtr = &soldier_array[i-1];
				info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2,
					(i>0 && soldierPtr->combat_level() < 20) ? (((20 - soldierPtr->combat_level()) <<6)+ 33) : 1);
			}
			else
			{
				// second round is ascending to draw the frame

				if( info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
					unitId, nation_recno,
					windowX1, windowY1, windowX2, windowY2, 
					inAreaFlag | (i==selected_soldier_id?3:0) ) & 4 )
				{
					inAreaFlag = 0;		// frame for mouse cursor is drawn, disable the frame
				}

				// display combat skill

				// ######## begin Gilbert 21/9 #######//
				Font *font = &font_whbl;

				if( !dispSpyMenu )
				{
					if( disp_combat_or_skill )		// display skill level
					{
						font = &font_blue;
						int attribute = -1;
						switch( disp_combat_or_skill )
						{
						case 1: 
							if( unitInfo->class_info.has_combat_level )
								attribute = combatLevel;
							break;
						case 2:
							if( unitInfo->class_info.has_skill_level && skillLevel > 0 )
								attribute = skillLevel; 
							break;
						case 4:
							if( unitInfo->class_info.has_loyalty && nation_recno )
								attribute = loyalty;
							break;
						default:
							err_here();
						}
						if( attribute >= 0 )	// hide attribute on some cases
							font->center_put( x, yHp, x+SOLDIER_X_SPACING, 
								yHp+font->max_font_height, m.format(attribute) );
					}
					else if( ownSpy )		// display spy icon
					{
						vga.active_buf->put_bitmap_trans( x+SOLDIER_X_SPACING/2-8, yHp-5, image_icon.read("U_SPY") );
					}
					else if( unitInfo->class_info.has_combat_level ) 	// display combat skill
					{
						font->center_put( x, yHp, x+SOLDIER_X_SPACING, yHp+font->max_font_height, 
							m.format(combatLevel) );
					}

					if( itemId )
					{
						char *iconPtr = item_res.item_unit_interface(itemId);
						if( iconPtr )
							vga.active_buf->put_bitmap_trans( x+SOLDIER_X_SPACING/2-((Bitmap *)iconPtr)->get_width()/2,
							yHp +53, iconPtr );
					}
				}
				else
				{
					// display spy skill
					err_when( !ownSpy );
					font_whbl.center_put( x, yHp, x+SOLDIER_X_SPACING, yHp+font->max_font_height, 
						m.format(spy_array[ownSpy]->spy_skill) );
				}

				// display hit points bar

				if( i > 0 && soldier_array[i-1].is_under_training() )
					disp_training_bar( x, yHp+font->max_font_height+2, x+SOLDIER_X_SPACING*7/8-1,
						soldier_array[i-1].skill_level(), BASIC_COMBAT_TRAIN );
				else
					disp_soldier_hit_points( x, yHp+font->max_font_height+2, x+SOLDIER_X_SPACING*7/8-1, hp, maxHp );
			}
		}
	}

/*
	//---------------- paint the panel --------------//

	if( overseer_recno )
	{
		//------------ display overseer info -------------//

		Unit* overseerUnit = unit_array[overseer_recno];

		int x=INFO_X1+6, y=dispY1+4, x1=x+UNIT_LARGE_ICON_WIDTH+8;

		if( selected_soldier_id == 0 )
		{
			vga_front.rect( x-2, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+1, 2, V_YELLOW );
		}
		else
		{
			vga.blt_buf( x-2, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y-1, 0 );
			vga.blt_buf( x-2, y+UNIT_LARGE_ICON_HEIGHT+1, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
			vga.blt_buf( x-2, y-2, x-1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
			vga.blt_buf( x+UNIT_LARGE_ICON_WIDTH, y-2, x+UNIT_LARGE_ICON_WIDTH+1, y+UNIT_LARGE_ICON_HEIGHT+2, 0 );
		}

		//-------------------------------------//

		if( refreshFlag == INFO_REPAINT )
		{
			vga_front.put_bitmap(x, y, unit_res[overseerUnit->unit_id]->get_large_icon_ptr(overseerUnit->rank_id) );
		}

		//-------- set help parameters --------//

		if( mouse.in_area(x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3) )
			help.set_unit_help( overseerUnit->unit_id, overseerUnit->rank_id, x, y, x+UNIT_LARGE_ICON_WIDTH+3, y+UNIT_LARGE_ICON_HEIGHT+3);

		//-------------------------------------//

		if( overseerUnit->rank_id == RANK_KING )
		{
			if( refreshFlag == INFO_REPAINT )
				font_san.put( x1, y, "King" );

			y+=14;
		}

		if( refreshFlag == INFO_REPAINT )
			font_san.put( x1, y, overseerUnit->unit_name(0), 0, INFO_X2-2 );		// 0-ask unit_name() not to return the title of the unit

		y+=14;

		//------- display leadership -------//

		String str;

		str  = translate.process("Leadership");
		str += ": ";
		str += overseerUnit->skill.skill_level;

		font_san.disp( x1, y, str, INFO_X2-10 );
		y+=14;

		//--------- display loyalty ----------//

		if( overseerUnit->rank_id != RANK_KING )
		{
			x1 = font_san.put( x1, y, "Loyalty:" );

			int x2 = info.disp_loyalty( x1, y-1, x1, overseerUnit->loyalty, overseerUnit->target_loyalty, nation_recno, refreshFlag );

			if( overseerUnit->spy_recno )
			{
				//------ if this is the player's spy -------//

				if( overseerUnit->is_own_spy() )
				{
					vga_front.put_bitmap( x2+5, y+1, image_icon.get_ptr("U_SPY") );
					x2 += 15;
				}
			}

			vga.blt_buf( x2, y-1, INFO_X2-2, dispY1+44, 0 );
		}
	}

	pop_disp_y1 = dispY1;

	//---------------- paint the panel --------------//

	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, dispY1, INFO_X2, dispY1+60 );

	//----------- display populatin distribution ---------//

	int overseerRaceId=0;

	if( overseer_recno )
		overseerRaceId = unit_array[overseer_recno]->race_id;

	if( selected_soldier_id > soldier_count )
		selected_soldier_id = soldier_count;

	//------ display population composition -------//

	int	  x, y;
	Soldier* soldierPtr = soldier_array;
	static  char last_race_id_array[MAX_SOLDIER];
	static  char last_unit_id_array[MAX_SOLDIER];

	dispY1+=1;

	for( int i=0 ; i<MAX_SOLDIER ; i++, soldierPtr++ )
	{
		x = INFO_X1+4+i%4*50;
		y = dispY1+i/4*29;

		if( i<soldier_count )
		{
			if( refreshFlag==INFO_REPAINT ||
				 last_race_id_array[i] != soldierPtr->race_id ||
				 last_unit_id_array[i] != soldierPtr->unit_id )
			{
				vga_front.put_bitmap(x+2, y+2, soldierPtr->small_icon_ptr());
			}

			//----- highlight the selected soldier -------//

			if( selected_soldier_id == i+1 )
				vga_front.rect( x, y, x+27, y+23, 2, V_YELLOW );
			else
				vga_front.rect( x, y, x+27, y+23, 2, vga_front.color_up );

			//------ display hit points bar --------//

			disp_soldier_hit_points( x+2, y+24, x+25, soldierPtr->shown_hit_points(), soldierPtr->max_hit_points() );

			//----- display combat or skill level ------//

			char* spyIconName=NULL;

			if( soldierPtr->spy_recno )
			{
				Spy* spyPtr = spy_array[soldierPtr->spy_recno];

				//------ if this is the player's spy -------//

				if( nation_array.player_recno &&
					 spyPtr->true_nation_recno == nation_array.player_recno )
				{
					spyIconName = "U_SPY";
				}

				//--------------------------------------------//
				//
				// If this is an enemy spy and this firm belongs
				// to the player and there is a player's phoenix
				// over this firm and the spying skill of the spy
				// is low (below 40)
				//
				//--------------------------------------------//

//				else if( spyPtr->spy_skill < 40 &&
//							nation_recno == nation_array.player_recno &&
//							nation_array.player_recno &&
//					 (~nation_array)->revealed_by_phoenix(loc_x1, loc_y1) )
//				{
//					spyIconName = "ENEMYSPY";
//				}

			}

			//--------------------------------------//

			if( spyIconName )
			{
				vga_front.put_bitmap( x+30, y+6, image_icon.get_ptr(spyIconName) );
				vga.blt_buf( x+40, y+6, x+49, y+15, 0 );
				vga.blt_buf( x+30, y+16, x+49, y+26, 0 );
			}
			else
			{
				font_san.disp(x+30, y+6, soldierPtr->skill.combat_level, 1, x+49);
			}

			last_race_id_array[i] = soldierPtr->race_id;
			last_unit_id_array[i] = soldierPtr->unit_id;

			//------- set help parameters ---------//

			if( mouse.in_area(x, y, x+27, y+23) )
				help.set_unit_help( soldierPtr->unit_id, 0, x, y, x+27, y+23 );
		}
		else
		{
			if( last_race_id_array[i] != 0 || last_unit_id_array[i] != 0 )
			{
				vga.blt_buf( x, y, x+49, y+27, 0 );
				last_race_id_array[i] = 0;
				last_unit_id_array[i] = 0;
			}
		}
	}
*/
}
//----------- End of function FirmCamp::disp_soldier_list -----------//


//--------- Begin of function FirmCamp::detect_soldier_list ---------//
//
// <int> selecteSpyMenu        0=main menu; 1=selecting spy
// when selectSpyMenu is 0, return 1 if left click on a unit, return 2 if right click on a unit
// when selectSpyMenu is 1, return spy_recno of the clicked spy, 0 if no own spy is clicked
//
int FirmCamp::detect_soldier_list(int selectSpyMenu)
{
	int dispY1 = disp_soldier_list_y1;

	// display in ascending order to select the overseer first

	for( int i = 0; i <= soldier_count; ++i )
	{
		// display soldier i

		int row = i/SOLDIER_PER_ROW;
		int x = INFO_X1 + 18 + (i % SOLDIER_PER_ROW) * SOLDIER_X_SPACING;
		int y = INFO_Y1 + 50 + row * SOLDIER_Y_SPACING;
		int yHp = INFO_Y1 + 7 + row * SOLDIER_Y_SPACING;

		int windowX1 = INFO_X1 + 16;
		int windowX2 = INFO_X1 + 220;
		int windowY1 = INFO_Y1 + 5 + row * 84;	// 5,89
		int windowY2 = windowY1 + 80 - 1 ;

		int unitId;
		int hp;
		int maxHp;
		// ##### begin Gilbert 21/9 ######//
		int combatLevel;
		int skillLevel;
		int loyalty;
		// ##### end Gilbert 21/9 ######//
		int ownSpy;

		if( i==0 )
		{
			if( !overseer_recno )
				continue;

			// overseer
			Unit *overseer = unit_array[overseer_recno];
			unitId = overseer->unit_id;

			hp = (int) overseer->hit_points;
			maxHp = overseer->max_hit_points();
			combatLevel = (int) overseer->combat_level();
			skillLevel = (int) overseer->skill_level();

			if( overseer->rank_id != RANK_GENERAL )
				loyalty = overseer->loyalty;
			else
				loyalty = -1;		// king or other(?)

			ownSpy = overseer->is_own_spy() ? overseer->spy_recno : 0;
		}
		else
		{
			// soldier
			Soldier *soldierPtr = &soldier_array[i-1];
			unitId = soldierPtr->unit_id;

			hp = soldierPtr->hit_points;
			maxHp = soldierPtr->max_hit_points();
			combatLevel = (int) soldierPtr->combat_level();
			skillLevel = (int) soldierPtr->skill_level();

			if( soldierPtr->race_id )
				loyalty = soldierPtr->loyalty;
			else
				loyalty = -1;
			ownSpy = soldierPtr->is_own_spy() ? soldierPtr->spy_recno : 0;
		}

		if( selectSpyMenu && !ownSpy )
			continue;

		int rc = info.draw_unit_icon( x+SOLDIER_X_SPACING/2, y,
			unitId, nation_recno, 
			windowX1, windowY1, windowX2, windowY2, 24 );		// detect left button (8) and right button(16)

		if( !rc )
			continue;

		if( selectSpyMenu == 0 )
		{
			// -------- main menu ---------//

			if( rc & 8 )
			{
				// ----- left click select soldier/overseer -------//

				selected_soldier_id = i;
				return 1;
			}
			else if( rc & 16 && is_own() )
			{
				// ------ right click mobilize solidier/overseer ------//

				if( i == 0 )
				{
					if(remote.is_enable())
					{
						// packet structure : <firm recno>
						short *shortPtr=(short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_OVERSEER, sizeof(short));
						shortPtr[0] = firm_recno;
					}
					else
					{
						assign_overseer(0);		// the overseer quits the camp
					}
				}
				else
				{
					// #### begin Gilbert 26/1 #####//
					if( !soldier_array[i-1].is_under_training() )
						mobilize_soldier(i, COMMAND_PLAYER);
					else
						cancel_train_soldier(i, COMMAND_PLAYER);
					// #### end Gilbert 26/1 #####//
				}
				return 2;
			}
		}
		else if( selectSpyMenu == 1 )
		{
			if( rc & 8 && ownSpy )
			{
				selected_soldier_id = i;
				return ownSpy;
			}
		}
	}

	return 0;

	/*
	if( !should_show_info() )
		return 0;

	if( is_own() )
	{
		//------ detect the overseer button -----//
		int rc = mouse.single_click(INFO_X1+6, INFO_Y1+58,
					INFO_X1+5+UNIT_LARGE_ICON_WIDTH, INFO_Y1+57+UNIT_LARGE_ICON_HEIGHT, 2 );

		if( rc==1 )
		{
			selected_soldier_id = 0;
			return 1;
		}
		else if( rc==2 && is_own() )
		{
			if(remote.is_enable())
			{
				// packet structure : <firm recno>
				short *shortPtr=(short *)remote.new_send_queue_msg(MSG_FIRM_MOBL_OVERSEER, sizeof(short));
				shortPtr[0] = firm_recno;
			}
			else
			{
				assign_overseer(0);		// the overseer quits the camp
			}
			return 1;
		}
	}

	//------- detect buttons on hiring firm soldiers -------//

	int i, x, y;

	for( i=0 ; i<soldier_count ; i++ )
	{
		x = INFO_X1+6+i%4*50;
		y = pop_disp_y1+1+i/4*29;

		switch( mouse.single_click(x, y, x+27, y+23, 2) )
		{
		case 1:         // left button to select soldier
			selected_soldier_id = i+1;
			return 1;

		case 2:
			if( is_own() )		// only if this is our own firm
			{
				//--- if the town where the unit lives belongs to the nation of this firm ---//

				mobilize_soldier(i+1, COMMAND_PLAYER);
				return 1;
			}
			break;
		}
	}

	return 0;
	*/
}
//----------- End of function FirmCamp::detect_soldier_list -----------//


//--------- Begin of function disp_soldier_hit_points ---------//
//
static void disp_soldier_hit_points(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
{
	//------- determine the hit bar type -------//

	#define HIT_BAR_TYPE_COUNT  3

	int  hit_bar_color_array[HIT_BAR_TYPE_COUNT] = { 0xA8, 0xB4, 0xAC };
	int  hit_bar_max_array[HIT_BAR_TYPE_COUNT] 	= { 50, 100, 200 };
	char hitBarColor;

	for( int i=0 ; i<HIT_BAR_TYPE_COUNT ; i++ )
	{
		if( maxHitPoints <= hit_bar_max_array[i] || i==HIT_BAR_TYPE_COUNT-1 )
		{
			hitBarColor = hit_bar_color_array[i];
			break;
		}
	}

	//------- draw the hit points bar -------//

	enum { HIT_BAR_DARK_BORDER = 3,
			 HIT_BAR_BODY 		   = 1 };

	int barWidth = (x2-x1+1) * hitPoints / max(hitPoints, maxHitPoints);

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

//	if( x1+barWidth <= x2 )
//		vga.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga.active_buf->bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga.active_buf->bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of function disp_soldier_hit_points -----------//


//--------- Begin of function disp_training_bar ---------//
//
static void disp_training_bar(int x1, int y1, int x2, int hitPoints, int maxHitPoints)
{
	//------- determine the hit bar type -------//

	int hitBarColor = 0xa4;	// blue

	//------- draw the hit points bar -------//

	enum { HIT_BAR_DARK_BORDER = 3,
			 HIT_BAR_BODY 		   = 1 };

	int barWidth = (x2-x1+1) * hitPoints / max(hitPoints, maxHitPoints);

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1+1, hitBarColor + HIT_BAR_BODY );

//	if( x1+barWidth <= x2 )
//		vga.blt_buf( x1+barWidth, y1, x2, y1+1, 0 );

	y1+=2;

	vga.active_buf->bar( x1, y1, x1+barWidth-1, y1, hitBarColor + HIT_BAR_DARK_BORDER );
	vga.active_buf->bar( x1+barWidth, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );

	y1++;

	vga.active_buf->bar( x1+1, y1, x1+barWidth, y1, V_BLACK );

//	if( x1+barWidth+1 <= x2 )
//		vga.blt_buf( x1+barWidth+1, y1, x2, y1, 0 );
}
//----------- End of function disp_training_bar -----------//


//--------- Begin of function FirmCamp::disp_soldier_info ---------//
//
void FirmCamp::disp_soldier_info(int dispY1, int refreshFlag)
{
	disp_soldier_info_y1 = dispY1;

	if( selected_soldier_id==0 )		// display overseer info
	{
		disp_overseer_info(dispY1, refreshFlag);
		return;
	}

	if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		int x=INFO_X1+20, y=dispY1;
		int x2;

		Soldier* soldierPtr = soldier_array + selected_soldier_id - 1;

		//------ if the unit is a living being -----//

		String str;

		if( soldierPtr->race_id )
		{
			if( soldierPtr->is_human() )
			{
				if( soldierPtr->hero_id )
					str  = hero_res[soldierPtr->hero_id]->name;
				else
					str  = race_res[soldierPtr->race_id]->get_name(soldierPtr->name_id);	// unit name
				str += " (";
				str += race_res[soldierPtr->race_id]->name;		// unit type name 
			}
			else
			{
				str  = monster_res.get_name(soldierPtr->name_id);		//monster name
				str += " (";
				str += monster_res[soldierPtr->monster_id()]->name;		// monster type name 
			}

			str += ")";
		}
		else
		{
			str = unit_res[soldierPtr->unit_id]->name;

			//------- if the unit is not a living being -----//

			// ###### begin Gilbert 24/3 #######//
			// if( unit_res[soldierPtr->unit_id]->unit_class == UNIT_CLASS_WEAPON )
			if( unit_res[soldierPtr->unit_id]->class_info.has_weapon_version
				&& soldierPtr->get_weapon_version() > 1 )
			// ###### end Gilbert 24/3 #######//
			{
				str += " ";
				str += m.roman_number( soldierPtr->get_weapon_version() );
			}
		}

		font_snds.put( x, y, str, 0, -1, 1 );

		//------------------------------------------------//

		// line spacing 24

		// ##### begin Gilbert 24/3 ##########//

		// ------- display loyalty ---------//

		UnitInfo *unitInfo = unit_res[soldierPtr->unit_id];

		if( unitInfo->class_info.has_loyalty && nation_recno )
		{
			if (soldierPtr->loyalty != soldierPtr->target_loyalty(firm_recno))
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(soldierPtr->loyalty, 4)) -
						font_snds.text_width(m.format(soldierPtr->target_loyalty(firm_recno), 4)) -
						font_snds.text_width("11"),
						soldierPtr->loyalty, soldierPtr->target_loyalty(firm_recno), nation_recno, refreshFlag, disp_combat_or_skill==4 );
			else
				info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(soldierPtr->loyalty, 4)),
						soldierPtr->loyalty, soldierPtr->target_loyalty(firm_recno), nation_recno, refreshFlag, disp_combat_or_skill==4 );
		}

		// ------- display combat ----------//

		if( unitInfo->class_info.has_combat_level )
		{
			x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, "Combat" ) + 10;
			font_snds.right_put( INFO_X2-10, y+12, m.format(soldierPtr->combat_level(),4) );
		}

		// ------- display leadership -------//

		if( unitInfo->class_info.has_skill_level )
		{
			x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, "Leadership" ) + 10;
			font_snds.right_put( INFO_X2-10, y+26, m.format(soldierPtr->skill_level(),4) );
		}

		// ##### end Gilbert 24/3 ##########//

		// ----- display hit point ---------//

		x2 = font_snds.put( x, y+26, "Hit Points" ) + 10;
		str = m.format(soldierPtr->hit_points, 4);
		str += "/";
		str += m.format(soldierPtr->max_hit_points(), 4);
		font_snds.right_put( INFO_X2-100, y+26, str );
	}
}
//--------- End of function FirmCamp::disp_soldier_info ---------//


//--------- Begin of function FirmCamp::detect_soldier_info ---------//
//
void FirmCamp::detect_soldier_info()
{
	int dispY1 = disp_soldier_info_y1;

	if( selected_soldier_id==0 )		// display overseer info
	{
		detect_overseer_info();
		return;
	}

	if( selected_soldier_id > 0 && selected_soldier_id <= soldier_count )
	{
		int x=INFO_X1+20, y=dispY1;
		Soldier *soldierPtr = soldier_array + selected_soldier_id - 1;
		UnitInfo *unitInfo = unit_res[soldierPtr->unit_id];

		if( unitInfo->class_info.has_loyalty && nation_recno
			&& mouse.single_click( x, y+14, x+79 , y+27 ) )
		{
			// detect loyalty
			disp_combat_or_skill = 4;
		}
		else if( unitInfo->class_info.has_combat_level
			&& mouse.single_click( x+110, y+14, x+189, y+27 ) )
		{
			// detect combat level
			disp_combat_or_skill = 1;
		}
		else if( unitInfo->class_info.has_skill_level
			&& soldierPtr->skill_level() > 0
			&& mouse.single_click( x+110, y+28, x+189, y+41 ) )
		{
			// detect skill
			disp_combat_or_skill = 2;
		}
		else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
		{
			disp_combat_or_skill = 0;
		}
	}
}
//--------- End of function FirmCamp::detect_soldier_info ---------//


//--------- Begin of function FirmCamp::disp_overseer_info ---------//
//
void FirmCamp::disp_overseer_info(int dispY1, int refreshFlag)
{
	disp_soldier_info_y1 = dispY1;

	if( !overseer_recno )
		return;

	Unit* unitPtr = unit_array[overseer_recno];
	UnitInfo *unitInfo = unit_res[unitPtr->unit_id];

	int x=INFO_X1+20, y=dispY1;
	int x2;

	// --- display name_id, can spot spy ----- //

	String str;

	str  = unitPtr->unit_name();
	str += " (";

	if( unitPtr->is_human() )
		str += race_res[unitPtr->race_id]->name;
	else
		str += monster_res[unitPtr->monster_id()]->name;

	str += ")";

	font_snds.put( x, y, str, 0, INFO_X2-8, 1 );

	// line spacing 24

	// ------- display loyalty ---------//

	if( unitInfo->class_info.has_loyalty && unitPtr->rank_id != RANK_KING && unitPtr->nation_recno )
	{
		err_when( unitPtr->unit_id == UNIT_WAGON );

		if (unitPtr->loyalty != unitPtr->target_loyalty)
			info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(unitPtr->loyalty, 4)) -
						font_snds.text_width(m.format(unitPtr->target_loyalty, 4)) -
						font_snds.text_width("11"),
						unitPtr->loyalty, unitPtr->target_loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
		else
			info.disp_loyalty( x, y+12, INFO_X2-99 - font_snds.text_width(m.format(unitPtr->loyalty, 4)),
						unitPtr->loyalty, unitPtr->target_loyalty, nation_recno, refreshFlag, disp_combat_or_skill==4 );
	}

	// ------- display combat ----------//

	if( unitInfo->class_info.has_combat_level )
	{
		x2 = (disp_combat_or_skill==1?font_blu2:font_snds).put( x+110, y+12, "Combat" ) + 10;
		font_snds.right_put( INFO_X2-10, y+12, m.format(unitPtr->combat_level(),4) );
	}
		
		// ------- display leadership -------//

	if( unitInfo->class_info.has_skill_level && unitPtr->skill_level() > 0 )
	{
		x2 = (disp_combat_or_skill==2?font_blu2:font_snds).put( x+110, y+26, "Leadership" ) + 10;
		font_snds.right_put( INFO_X2-10, y+26, m.format(unitPtr->skill_level(),4) );
	}

	// ----- display hit point ---------//

	x2 = font_snds.put( x, y+26, "Hit Points" ) + 10;
	str = m.format((int)unitPtr->hit_points, 4);
	str += "/";
	str += m.format(unitPtr->max_hit_points(), 4);
	font_snds.right_put( INFO_X2-100, y+26, str );
}
//----------- End of function FirmCamp::disp_overseer_info -----------//


//--------- Begin of function FirmCamp::detect_overseer_info ---------//
//
void FirmCamp::detect_overseer_info()
{
	int dispY1 = disp_soldier_info_y1;
	if( !overseer_recno )
		return;

	int x=INFO_X1+20, y=dispY1;

	Unit *unitPtr = unit_array[overseer_recno];
	UnitInfo *unitInfo = unit_res[unitPtr->unit_id];

	if( unitInfo->class_info.has_loyalty && nation_recno && unitPtr->rank_id != RANK_KING
		&& mouse.single_click( x, y+14, x+79 , y+27 ) )
	{
		// detect loyalty
		disp_combat_or_skill = 4;
	}
	else if( unitInfo->class_info.has_combat_level
		&& mouse.single_click( x+110, y+14, x+189, y+27 ) )
	{
		// detect combat level
		disp_combat_or_skill = 1;
	}
	else if( unitPtr->skill_level() > 0
		&& mouse.single_click( x+110, y+28, x+189, y+41 ) )
	{
		// detect skill
		disp_combat_or_skill = 2;
	}
	else if( mouse.single_click( x-4, y-4, x+200, y+47 ) )
	{
		disp_combat_or_skill = 0;
	}
}
//--------- End of function FirmCamp::detect_overseer_info ---------//


//--------- Begin of function FirmCamp::disp_train_menu ---------//
//
void FirmCamp::disp_train_menu(int refreshFlag)
{
	err_here();
}
//--------- End of function FirmCamp::disp_train_menu ---------//


//--------- Begin of function FirmCamp::detect_train_menu ---------//
//
void FirmCamp::detect_train_menu()
{
	err_here();
}
//--------- End of function FirmCamp::detect_train_menu ---------//


#ifdef DEBUG

//----------- Begin of static function disp_debug_info -----------//

static void disp_debug_info(FirmCamp* firmPtr, int refreshFlag)
{
	if( refreshFlag == INFO_REPAINT )
		vga.d3_panel_up( INFO_X1, INFO_Y2-40, INFO_X2, INFO_Y2 );

	int x=INFO_X1+3, y=INFO_Y2-37, x2=x+120;

	font_san.field( x, y   , "patrol unit count", x2, firmPtr->patrol_unit_count, 1, INFO_X2, refreshFlag);
	font_san.field( x, y+16, "coming unit count", x2, firmPtr->coming_unit_count, 1, INFO_X2, refreshFlag);

	font_san.put( x+180, y, firmPtr->firm_recno );
}
//----------- End of static function disp_debug_info -----------//

#endif


// ---- begin of function FirmCamp::explore_for_player --------//
//
int FirmCamp::explore_for_player()
{
	return Firm::explore_for_player()
		|| player_spy_count > 0;
}
// ---- end of function FirmCamp::explore_for_player --------//
