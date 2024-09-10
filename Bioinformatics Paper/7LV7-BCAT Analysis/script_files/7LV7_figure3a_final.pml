fetch 7lv7A
set cartoon_transparency, 0.25
remove solvent
select edy, resn EDO
remove edy
set opaque_background, on
bg_color white
color gray
select LLPs, resn LLP
show sticks, LLPs
util.cbap LLPs

set_view (\
     0.637408435,    0.240524054,    0.732004881,\
     0.725654006,   -0.506795347,   -0.465364993,\
     0.259034932,    0.827815294,   -0.497578830,\
    -0.000431173,   -0.000664538, -165.993988037,\
    -0.039622784,   -1.788363934,   22.063785553,\
  -31007.011718750, 31339.111328125,  -20.000000000 )
