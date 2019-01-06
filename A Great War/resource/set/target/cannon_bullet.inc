{selector
  {type "cannon_bullet"}

  {group
    {select {class car}}
    {select {class human}}
    {sort {weapon "fg"}}
    {sort {weapon "ap"}}
    {sort {weapon "apcr"}}
    {sort {weapon}}
    {sort distance}
  }

  {group
    {sort {attacking}}
    {sort distance}
  }
}
