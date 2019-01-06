; == STANDARD ==
(define "standard_all_8v8"
	{common
		{unitset 
			{value "all^arcade"}
		}
		{commandPoints "*=%cp"}
		{unitLimits "flamer=3;sniper_only=2;officer=1;heavyaa=1;single=20"}
		{balancingFactorBonus {cp 1.0} {mp 1.0} }
		{chargeFactors
			{value "*=2.0"}
		}
		{budgets
			{vehicle
				{mode "standard"}
				{resource 
					{start 		%start}
					{finish 	%finish}
					{growTime	1800}		; in seconds
					{growTable 	"0.65 0.65 0.65 0.70 0.70 0.70 0.75 0.75 0.75 0.80 0.80 0.80 0.80 0.80 0.80 0.85 0.85 0.85 0.90 0.90 0.90 0.90 0.90 0.90 1.00"}
					{payback
						{cp 		"0		25		50 		60		70		80		90"}  
						{restore 	"1.50	1.25	0.75	0.50	0.40	0.30	0.20"}
					}
				} 
				{buttons "squad1 squad2 v1 v2 v3 v4 v5"}
			}
			{hero
				{resource 
					{start 		10}
				} 
				{buttons "hero"}
				{tag "special"}
			}
		} 
	}
	{bots
		(include "bots_generic.inc")
	}
)


;------------------------------------------------------------------------------------------------------------
{presets
	{"s:standard_50_all_8v8"	
		{gameMode "standard"}
		{unitMode "all_8v8"}
		{specMode "all_8v8_normal"}
		("standard_all_8v8" 		start(160) finish(2500) cp(50))
		{ranked}
	}
;------------------------------------------------------------------------------------------------------------
}