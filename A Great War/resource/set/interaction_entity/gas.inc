{"chlorine_gas_red_star"
	{on spawn
		{add_view "chlorine_gas_cloud" "gas_cloud" "basis" }
		{view start "gas_cloud"}
		{delay 2
			{add_view "chlorine_gas" "engi" "basis" }
			{view start "engi"}
		}
		{delay 1800
			{delete}
		}
	}
}