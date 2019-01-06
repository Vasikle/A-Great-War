{"airborne"
;	{friction 0.2}
;	{restitution 0.01}
	{on spawn
		{link_sound "fall"				"vehicle/airborne/fw_deep"			}

		{add_view	"exhaust_dark_big"	"engine_exhaust"		"FXsmoke1"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust" 		"FXsmoke2"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust"		"FXsmoke3"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust" 		"FXsmoke4"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust"		"FXsmoke5"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust" 		"FXsmoke6"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust"		"FXsmoke7"	}
		{add_view	"exhaust_dark_big"	"engine_exhaust" 		"FXsmoke8"	}
		{call "init_gun"}
	}
	{on "add_movement_sound"
		{link_sound "fly"				"vehicle/airborne/fw_move"			}
		{link_sound "engine_start"		"vehicle/airborne/fw_start"			}
		{link_sound "engine_work"		"vehicle/airborne/fw_work"			}
		{link_sound "engine_stop"		"vehicle/airborne/fw_stop"			}
	}
	{on "takeoff"
		{if not dead
			{ani_play "wheels" 0.2}
			{play_sound "fly" 1}
		}
	}
	{on "takeoff_load"
		{ani_wind "wheels" end}
	}
	{on "landing"
		{ani_play "wheels" -0.2}
		{stop_sound "fly"}
	}
	{on engine on overload
		{call "engine_on"}
	}
	{on animation_end "engine_start"
		{ani_play "engine_work" loop}
		{play_sound "engine_work" 1}
	}
	{on engine off overload
		{call "engine_stop"}
		{play_sound "engine_stop"}
	}
	{on "engine_on"
		{if not "engine_started"
			{set "engine_started" 1}
			{play_sound "engine_start"}
			{ani_play "engine_start" callback}
			{view start "engine_exhaust"}
			{delay 1
				{view pause "engine_exhaust"}
			}
		}
	}
	{on "update_moveable" overload
		{if not chassis "airborne"
			{inherited}
		}
	}
	{on "engine_stop"
		{kill_delay "engine_stopped"}
		{if "engine_started"
			{set "engine_started" 0}
			{ani_stop "engine_start"}
			{ani_stop "engine_work"}
			{ani_play "engine_stop"}
			{stop_sound "engine_start"}
			{stop_sound "engine_work"}
			{stop_sound "fly"}
		}
	}
	{on break_armor
		{if name "pierce"
			{if stuff "shell"
				{call "pierce"}
			else stuff "zenite"
				{if rand 0.3
					{call "pierce"}
				}
			}
		else name "blast"
			{if volume "body"
				{impulse up 1 0.5 dir 0.3 0.2}
				{call "explosion"}
			}
		}
	}
	{on "pierce"
		{if work
			{component break}
			{spawn "aero_die_particle"}
			{if volume "engine"
				{burn fx "smoke_aero_big" fake}
			else volume "body"
				{burn fx "smoke_aero_big" fake}
			else;volume "inflammable"
				{burn fx "smoke_aero_small" fake}
			}
		}
	}
	{on break
		{if chassis "airborne"
			{if component "body"
				{call "fall"}
			else component "engine"
				{call "fall"}
			}
		}
	}
	{on "fall"
		{if not "fall"
			{set "fall" 1}
			{impulse up 0.1 com}
			{explosive 1}
			{play_sound "fall"}
			{call "fall_fx"}
		}
	}
	{on "fall_fx"
		{delay 2
			{add_view "smoke_aero_dead_big" "fire" "body"}
			{view start "fire"}
			{delay 10
				{view stop "fire"}
			}
			{burn volume "body" fx "smoke_aero_big" time 10 fake}
			{burn volume "engine" fx "smoke_aero_big" time 10 fake}
			{delay 10.1
				{burn volume "body" fx "smoke_black" time 40 10 fake}
			}
;			{burn volume "wingr1" fx "smoke_aero_big" time 5 fake}
;			{burn volume "wingl1" fx "smoke_aero_big" time 5 fake}
		}
	}
	{on ground_hit
		{call "collide"}
		{delay 2.5 1
			{call "explosion"}
		}
	}
	{on contact overload
		{if effector "vehicle"
			{call "collide"}
		else effector "construction"
			{call "collide"}
		else effector "tree"
			{call "collide"}
		else effector "fence"
			{if volume "part"
				{call "tear_part"}
			}
		}
	}
	{on "collide"
		{if volume "body"
			{call "explosion"}
		else volume "part"
			{call "tear_part"}
		}
	}
	{on "tear_part"
		{spawn "spheredust_big_short"}
		{component destroy}
		{components "engine" destroy}
		{volume disable contact contact_ground}
		{chassis_work 0}
		{tear "piece_big_airborne_wing" bone from_volume
			{impulse up 2 1 dir 2 1}
		}
		{impulse up 0.1 com}
	}

	(define "burn_part"
		{on "burn_%0_start"
			{add_view "smoke_aero_big"		"fire%0" "fx_fire%0_1"}
			{add_view "smoke_aero_big"		"fire%0" "fx_fire%0_2"}
			{add_view "smoke_aero_big"		"fire%0" "fx_fire%0_3"}
			{add_view "smoke_aero_small"	"fire%0" "fx_smoke%0_1"}
			{add_view "smoke_aero_small"	"fire%0" "fx_smoke%0_2"}
			{add_view "smoke_aero_small"	"fire%0" "fx_smoke%0_3"}
			{view start "fire%0"}
		}
		{on "burn_%0_stop"
			{view stop "fire%0"}
		}
	)
	("burn_part" args 1)
	("burn_part" args 2)
	("burn_part" args 3)
	("burn_part" args 4)

	{on "die"
		{chassis_work 0}
		{drop_orders}
		{components destroy}
		{call "engine_stop"}
		{stop_sound "fall"}
		{stop_sound "engine_stop"}
		{explosive 0}
		{burn stop}
		{bone hide "engine_work"}
	}
	{on "_explosion" overload
		{kill_crew}
		{able select 0}
		{clear_inventory}
		{call "die"}
		{add_view "aero_die" "fire" "body"}
		{add_view "aero_die_fire" "fire" "body"}
		{view start "fire"}
		{spawn "aero_die_particle"}
		{start_sound "crush/big/airborne"}
		{tags add "destroyed"}
		{tags remove "air"}

		{impulse up 0.1 com}
		{delay 0.1
			(define "cut_part"
				{if rand %1
					{tear "piece_big_airborne_wing" bone %0
						{impulse up 2 1 dir 2 1}
					}
				}
			)
			("cut_part" args "wingr1"		0.5)
			("cut_part" args "wingl1"		0.5)
			("cut_part" args "wingr2"		0.5)
			("cut_part" args "wingl2"		0.5)
			("cut_part" args "body_back"	0.5)
			("cut_part" args "propeller"	1.0)
			{volumes disable obstacle}
			{volumes "body" enable obstacle}
			{volumes "body_back" disable obstacle}
			{area rebuild}
		}
		{blastwave c4 7 r0 5 r1 8}
	}
	{on "spawn_bomb"
		{spawn "air_bomb_small"}
	}
	{on "kick"
		{impulse up 0.1 com}
	}
	{on "area_disable"
		{area disable}
	}
	{on "area_enable"
		{area enable}
	}
	{on "air_attack"
		{start_sound "vehicle/airborne/air_attack" ambient}
	}

	{on "init_gun"
		{add_view "flash_zenit"	"flashbarrel00"	"barrel00"}
		{add_view "flash_zenit"	"flashbarrel01"	"barrel01"}
		{add_view "flash_zenit"	"flashbarrel02"	"barrel02"}
		{add_view "flash_zenit"	"flashbarrel03"	"barrel03"}
		{add_view "flash_zenit"	"flashbarrel04"	"barrel04"}
	}
	{on fire "zenite"
		(define "fire_barrel"
			name "%0"
				{view start "flashbarrel%0"}
				{delay 0.42
					{view pause "flashbarrel%0"}
				}
		)
		
		{if    ("fire_barrel" args 00)
		  else ("fire_barrel" args 01)
		  else ("fire_barrel" args 02)
		  else ("fire_barrel" args 03)
		  else ("fire_barrel" args 04)
		}
	}
}

{"airborne no_explosion"
	{on contact
;		{components "wingleft" destroy}
;		{components "wingright" destroy}
	}
	{on ground_hit
		{call "die"}
		{impulse up 0.1 com}
	}
	{on "_explosion" overload
		{call "die"}
		{impulse up 0.1 com}
		("cut_part" args "wingr2"		0.5)
		("cut_part" args "wingl2"		0.5)
	}
	{on "fall_fx" overload
	}
}

{"air_part"
	{on spawn
		{delay 0.1
			{volumes enable contact contact_ground}
		}
		{view stop ""}
		{add_view "smoke_black" "smoke" "root"}
		{view start "smoke"}
		{delay 180 30
			{view stop "smoke"}
		}
	}
	{on ground_hit
		{if min_speed 5
			{spawn "spheredust_big_short"}
		}
	}
}

{"air_part wing"
	{on ground_hit
		{if volume "part"
			{if volume_bone_level 3
				{call "tear_part"}
			}
		}
	}
	{on "tear_part"
		{tear "piece_big_airborne_wing" bone from_volume
			{impulse up 0.1 0.05}
		}
	}
}

{"airborne P-51Mustang"
	{on "add_movement_sound" overload
		{link_sound "fly"				"vehicle/airborne/p51_move"			}
		{link_sound "engine_start"		"vehicle/airborne/p51_start"		}
		{link_sound "engine_work"		"vehicle/airborne/p51_work"			}
		{link_sound "engine_stop"		"vehicle/airborne/fw_stop"			}
	}
	{on "update_lights" overload}
}

{"airborne cg-4a"
	{on spawn
		{add_view	"smoke_plane"	"smoke01"		"fx_smoke01"}
		{add_view	"smoke_plane"	"smoke02"		"fx_smoke02"}
		{add_view	"smoke_plane"	"smoke03"		"fx_smoke03"}
		{add_view	"smoke_plane"	"smoke04"		"fx_smoke04"}
		{add_view	"smoke_plane"	"smoke05"		"fx_smoke05"}
		{add_view	"smoke_plane"	"smoke06"		"fx_smoke06"}
		{add_view	"smoke_plane"	"smoke07"		"fx_smoke07"}
		{add_view	"smoke_plane"	"smoke08"		"fx_smoke08"}
		{add_view	"smoke_plane"	"smoke09"		"fx_smoke09"}
		{add_view	"smoke_plane"	"smoke10"		"fx_smoke10"}
		{add_view	"smoke_plane"	"smoke11"		"fx_smoke11"}
		{add_view	"smoke_plane"	"smoke12"		"fx_smoke12"}
	}
	{on "tear_part"
		{view start "smoke01"}
		{view start "smoke02"}
		{view start "smoke03"}
		{view start "smoke04"}
		{view start "smoke05"}
		{view start "smoke06"}
		{view start "smoke07"}
		{view start "smoke08"}
		{view start "smoke09"}
		{view start "smoke10"}
		{view start "smoke11"}
		{view start "smoke12"}
	}
	{on "add_movement_sound" overload
;		{link_sound "fly"				"vehicle/airborne/cg_move"			}
		{link_sound "engine_start"		"vehicle/airborne/cg_start"			}
		{link_sound "engine_work"		"vehicle/airborne/cg"			}
;		{link_sound "engine_stop"		"vehicle/airborne/cg_stop"			}
	}
}

{"hamilcar"
	{on spawn
		{add_view	"smoke_plane"	"smoke01"		"fx_smoke01"}
		{add_view	"smoke_plane"	"smoke02"		"fx_smoke02"}
		{add_view	"smoke_plane"	"smoke03"		"fx_smoke03"}
		{add_view	"smoke_plane"	"smoke04"		"fx_smoke04"}
		{add_view	"smoke_plane"	"smoke05"		"fx_smoke05"}
		{add_view	"smoke_plane"	"smoke06"		"fx_smoke06"}
		{add_view	"smoke_plane"	"smoke07"		"fx_smoke07"}
		{add_view	"smoke_plane"	"smoke08"		"fx_smoke08"}
		{add_view	"smoke_plane"	"smoke09"		"fx_smoke09"}
		{add_view	"smoke_plane"	"smoke10"		"fx_smoke10"}
		{add_view	"smoke_plane"	"smoke11"		"fx_smoke11"}
		{add_view	"smoke_plane"	"smoke12"		"fx_smoke12"}
	}
	{on "tear_part"
		{view start "smoke01"}
		{view start "smoke02"}
		{view start "smoke03"}
		{view start "smoke04"}
		{view start "smoke05"}
		{view start "smoke06"}
		{view start "smoke07"}
		{view start "smoke08"}
		{view start "smoke09"}
		{view start "smoke10"}
		{view start "smoke11"}
		{view start "smoke12"}
	}
	{on "add_movement_sound" overload
;		{link_sound "fly"				"vehicle/airborne/cg_move"			}
		{link_sound "engine_start"		"vehicle/airborne/cg_start"			}
		{link_sound "engine_work"		"vehicle/airborne/cg"			}
;		{link_sound "engine_stop"		"vehicle/airborne/cg_stop"			}
	}
	{on "cut_wheels"
		{ani_play "drift"}
		("cut_part" args "wheell"		1.0)
		("cut_part" args "wheelr"		1.0)
		("cut_part" args "wheel_back"	1.0)
;		("cut_part" args "part2"		0.7)
		("cut_part" args "part3"		1.0)
		{add_view "spheredust_big" "ground_dust" "support1"}
		{add_view "spheredust_big" "ground_dust" "support2"}
		{view start "ground_dust"}
		{delay 4
			{view stop "ground_dust"}
		}
	}
}

{"il-2"
	{on "die"
		{bone hide "rocket00"}
		{bone hide "rocket01"}
		{bone hide "rocket02"}
		{bone hide "rocket03"}
		{bone hide "rocket04"}
		{bone hide "rocket05"}
		{bone hide "rocket06"}
		{bone hide "rocket07"}
		{bone hide "bomb00"}
		{bone hide "bomb01"}
	}
}

{"ju-87"
	{on "die"
		{bone hide "bomb00"}
		{bone hide "bomb01"}
		{bone hide "bomb02"}
		{bone hide "bomb03"}
	}
}

{"ju-52"
	{on "wheels_fx"
		{add_view "spheredust_big" "ground_dust" "support1"}
		{add_view "spheredust_big" "ground_dust" "support2"}
		{view start "ground_dust"}
		{delay 4
			{view stop "ground_dust"}
		}
	}
}


{"bomber"
	{on "init_gun" overload
		{add_view "flash_gun_big_vehicle" "shooter1_00" "shooter1_00"}
		{add_view "flash_gun_big_vehicle" "shooter1_01" "shooter1_01"}
		{add_view "flash_gun_big_vehicle" "shooter2_00" "shooter2_00"}
		{add_view "flash_gun_big_vehicle" "shooter2_01" "shooter2_01"}
		{add_view "flash_gun_big_vehicle" "shooter3_00" "shooter3_00"}
		{add_view "flash_gun_big_vehicle" "shooter3_01" "shooter3_01"}
		{add_view "flash_gun_big_vehicle" "shooter4_00" "shooter4_00"}
		{add_view "flash_gun_big_vehicle" "shooter4_01" "shooter4_01"}
	}
	(define "link_flash"
		{if name "mgun_linked%0"
			{if stuff "mgun"
				{add_view "flash_gun_big_vehicle" "flashbarrel%1" "Foresight%1"}
			}
		}
	)
	{on "link_weapon" overload
		("link_flash" args 1 5)
		("link_flash" args 2 6)
		("link_flash" args 3 7)
		("link_flash" args 4 8)
	}
	{on fire "mgun_linked1" {view start "flashbarrel5"}}
	{on fire "mgun_linked2" {view start "flashbarrel6"}}
	{on fire "mgun_linked3" {view start "flashbarrel7"}}
	{on fire "mgun_linked4" {view start "flashbarrel8"}}

	(define "start_shooter_flash"
		{on fire "mgun_own%0"
			{if name "00"
				{view start "shooter%0_00"}
			}
			{if name "01"
				{view start "shooter%0_01"}
			}
		}
	)
	("start_shooter_flash" args 1)
	("start_shooter_flash" args 2)
	("start_shooter_flash" args 3)
	("start_shooter_flash" args 4)

	{on "_explosion" overload}

	{on "break_part"
		{if work
			{component break}
			{burn fx "smoke_aero_small" fake}
		else broken
			{component destroy}
			{spawn "aero_die_particle"}
			{if volume "engine"
				{tear "piece_medium_steel" bone from_volume
					{impulse up 1 0.5 dir 5 2 rnd_dir 3 no_position}
					{add_view "smoke_dead_wheel" "smoke" "basis"}
					{view start "smoke"}
					{delay_effect 20 5 "stop"}
				}
			}
		else destroyed
			{spawn "aero_die_particle"}
		}
	}

	{on "pierce" overload
		{if volume "engine"
			{call "break_part"}
		else volume "wing"
			{call "break_part"}
		else volume "body"
			{call "break_part"}
		else
			{inherited}
		}
	}
}
