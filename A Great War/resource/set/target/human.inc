{selector
	{unit "human"}

	{group
		{sort {in_range 10}}
		{sort {attacking}}
		{sort {class human} {weapon "bullet"}}
		{sort {class human} {weapon}}
		{sort {class tank}}
		{sort {class armoredcar}}
		{sort {weapon}}
		{sort {class human}}
		{sort distance}
	}
	{group
		{sort distance}
	}
}