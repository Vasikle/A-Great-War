{selector
	{type "tank"}
	{group
		{ignore {class airborne}}
		{select {weapon "ap"}}
		{select {weapon "fg"}}		
		{select {weapon "apcr"}}

		{sort {weapon "ap"} {attacking} {class tank}}
		{sort {weapon "ap"} {attacking}}
		{sort {weapon "ap"} {threating} {class tank}}
		{sort {weapon "ap"} {threating}}
		{sort {weapon "ap"} {class tank}}
		{sort {weapon "ap"}}
		{sort {weapon "fg"} {attacking}}
		{sort {weapon "fg"} {threating}}
		{sort {weapon "fg"}}
		{sort time_to_aim}
	}
	{group
		{sort {class tank}}
		{sort {class armoredcar}}
		{sort {class car}}
		{sort time_to_aim}
	}

}
