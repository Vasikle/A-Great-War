;= START =====================================


(define "vision_human"
    ("radius" r(70))
    {rules
        {human 1
            {firing 1.15
				("firing")
            }
			("vision")
        }
		("cannon")
		("tank")
    }
    {mods
        {stand 1}
    }
)

(define "vision_humancrew"
    ("radius" r(40))
    {rules
        {human 1
            {firing 1.10
				("firing")
            }
			("vision")
        }
		("cannon")
		("tank")
    }
    {mods
        {stand 1}
    }
)

(define "vision_optic"
    ("radius" r(90))
    {rules
        {human 0.75
            {firing 0.85
				("firing")
            }
			("vision")
        }
		("cannon")
		("tank")
    }
    {mods
        {stand 1}
    }
)

(define "vision_gunner"
    ("radius" r(40))
    {rules
        {human 0.75
            {firing 0.85
				("firing")
            }
			("vision")
        }
		("cannon")
		("tank")
    }
    {mods
        {stand 1}
    }
)

(define "vision_turret"
    ("radius" r(40))
    {rules
        {human 0.75
            {firing 0.85
				("firing")
            }
			("vision")
        }
		("cannon")
		("tank")
    }
    {mods
        {stand 1}
    }
)

(define "glass"
		("radius" r(%0))
        {rules
            {zero 0}
        }
)

;= MAP HELPER ====================================

(define "ultra_vision"
	{radius 20}
	{h_fov 360}
   	{v_fov 180}
	{uncover %range}
)
{"start_point"  ("ultra_vision" range(20))}
{"map_point"    ("ultra_vision" range(20))}
{"flag_point"   ("ultra_vision" range(20))}
