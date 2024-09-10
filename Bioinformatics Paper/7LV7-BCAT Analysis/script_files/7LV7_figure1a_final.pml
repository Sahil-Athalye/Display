fetch 7LV7A
remove solvent
select dom1, resi -1-162
select dom2, resi 163-355
color red, dom1
color cyan, dom2
remove resn EDO
bg_color white
set_view (\
     0.281319171,   -0.414009303,   -0.865708947,\
     0.835294247,   -0.338439077,    0.433289826,\
    -0.472374558,   -0.845015585,    0.250609368,\
     0.000000000,   -0.000000000, -255.730865479,\
     8.957004547,  -15.050493240,   12.628124237,\
   169.071166992,  342.390563965,  -20.000000000 )
show sticks, resn LLP
color black, resn LLP
