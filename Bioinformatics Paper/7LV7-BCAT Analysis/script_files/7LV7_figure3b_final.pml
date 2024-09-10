fetch 7lv7A
set cartoon_transparency, 0.5
remove solvent
select edy, resn EDO
remove resn EDO
set opaque_background, on
bg_color white
util.cbaw
select LLPs, resn LLP
util.cbap LLPs
show sticks, LLPs
distance dist, LLPs, 7lv7A, 3.5, mode=2
color yellow, name CL
color red, resi 297+60+261+260+84+298+259+192+88
show sticks, resi 297+60+261+260+84+298+259+192+88
set cartoon_transparency, 0, resi 297+60+261+260+84+298+259+192+88
hide labels
set_view (\
     0.860011101,    0.196566969,    0.470866650,\
     0.399023533,   -0.834230006,   -0.380551666,\
     0.317999065,    0.515171289,   -0.795886874,\
    -0.000537897,   -0.000476260,  -61.550701141,\
     4.440359592,   -8.086267471,   20.393438339,\
   -24.960636139,  148.358673096,  -20.000000000 )

