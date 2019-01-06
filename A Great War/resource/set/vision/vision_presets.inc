(define "firing"
	{welrod 0.25}
	{nagan_pb 0.25}
	{delisle 0.25}
	{hdm 0.25}
	{sten_v_silencer 0.25}
	{knife_ger 0}
	{knife_rus 0}
	{knife_union 0}
	{knife_japan 0}
	{axe 0}	
	{crow_bar 0}
	{smoke 0.1}
	{smoke_paradrop_ammo 0.1}
	{smoke_paradrop_ammo_heavy 0.1}
	{f1 0.1}
	{m24 0.1}
	{m39 0.1}
	{m61 0.1}
	{mk1 0.1}
	{satchel 0.1}
	{type97 0.1}
	{type99 0.1}
	{m24x5 0.1}
	{molotov_coctail 0.1}
	{n73at 0.1}
	{pwm1 0.1}
	{rg42x3 0.1}
	{rpg40 0.1}
	{rpg43 0.1}
	{type3at 0.1}
	{type3at_a 0.1}
	{type99at 0.1}
)
(define "vision"
	{cover 0.80
		{stand 0.80
			{stealth 0.40}		; 50%
		}
		{squat 0.60
			{stealth 0.30}		; 50%
		}
		{crawl 0.40
			{stealth 0.20}		; 50%
		}
    }
    {squat 0.80
        {stealth 0.40}			; 50%
	}
    {crawl 0.60
        {stealth 0.30}			; 50%
        {still 0.40
            {stealth 0.20}		; 50%
        }
    }
    {still 0.9}
    {visible 1.0}
)

(define "cannon"
        {cannon_01 1
            {firing 1.1}
            {visible 1}
            {still 1}
        }
        {cannon_02 1.1
            {firing 1.2}
            {visible 1.1}
            {still 1.1}
        }
        {cannon_03 1.2
            {firing 1.3}
            {visible 1.2}
            {still 1.2}
        }
        {cannon_04 1.3
            {firing 1.4}
            {visible 1.3}
            {still 1.3}
        }
        {cannon_05 1.4
            {firing 1.5}
            {visible 1.4}
            {still 1.4}
        }
)
(define "tank"
        {vehicle_01 1.1
            {firing 2.0}
            {visible 1.6}
            {still 1.6}
        }
        {vehicle_02 1.8
            {firing 2.25}
            {visible 1.7}
            {still 1.7}
        }
        {vehicle_03 2.0
            {firing 2.5}
            {visible 1.8}
            {still 1.8}
        }
        {vehicle_04 2.2
            {firing 2.75}
            {visible 1.9}
            {still 1.9}
        }
        {vehicle_05 2.4
            {firing 3}
            {visible 2}
            {still 2}
        }
)