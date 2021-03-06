;SDL
(include "/properties/moto.ext")
{props "bike" "explosion_throw_crew" "turn_over" "vision_lev01"}
{able "emit_on_attack"}
(mod "mp"
	{able "-emit_on_attack"}
)
{patherId "moto"}

{sensor
	{Visor "main"
		{vision "vehicle_main"}
		{bone "visor1"}
	}
	{Visor "crew"
		{vision "moto_crew"}
		{bone "visor2"}
	}
	(mod "mp"
         {visor "view-around"
                {vision "moto_around"}
                {bone "basis"}
         }
    )
}

{Placer
	{place "driver" {Group "driver"} {LinkBone "driver"} {visor "main"} {turnoff {sensor}{shadow}}}
}
{boarder
	{door "emit1"}
	
	{anm "driver" {forward  {end "seat_driver_moto"}}}
	
	{link "emit1" "driver"    {anm "driver"} {forward putoff} {reverse puton}}
	}

("wheel" args "wheel1")
("wheel" args "wheel2")
("wheel" args "wheel3")

{extender "inventory"
	
	{Box
		{item  "ammo rifle" 72 }
	}
}
{mass 100}
{Chassis
	{Locomotion ("locomotion")
		{MaxSpeed	20}
				(mod "mp"
					{maxspeed   17}
				)			; ���ᨬ��쭠� ᪮����, kmph
	  	{maxSpeedAtMaxTurn 15}	
		{StartTime	3	}		; �६� ࠧ���� �� ����. ᪮��� � 0, seconds
		{BrakeTime	2	}		; �६� �ମ����� � ����. ᪮��� �� 0, seconds
		{StopTime	4	}		; �६� ��⠭���� ��᫥ �⪫�祭�� �����⥫� � ����. ᪮��� �� 0, seconds
		{TurnRadius	3	}		; ���. ࠤ��� ࠧ����, meter
		{Gears		0.35 0.5 0.7 1.0} ; ��।���� ����-�� ��஡�� ��।��
		{RearGears	0.33 0.5}	; ��।���� ����-�� ��஡�� ��।��
		{SteerOn	30}			; ᪮���� ������ �����, �ࠤ�ᮢ � ᥪ㭤�
		{SteerOff	40}         ; ᪮���� ������ ����� � ���. ���ﭨ�, �ࠤ�ᮢ � ᥪ㭤�
		{SplineRadius 2}		; ��࠭�祭�� ࠤ��� ������ �� ᯫ�����, ���஢
	}	
	{FuelBag
		{volume 50 }	; liters
		{rate	10 }   	; liters per kilometer
		{remain 50 }
		{fuel "fuel"}
	}
	{speed
		{Slow	3	}	; km/h
	}
}

{Weaponry
	
}  	
{Extension "bike.mdl"}
	{bone "gun_rot"
		{limits -45 35}
	}