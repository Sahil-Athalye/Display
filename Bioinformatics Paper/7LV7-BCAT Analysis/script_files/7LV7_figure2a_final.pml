#This gets me my alignment file
#fetch 7LV7A 4DQNA 3UYYA 6JIFA 3HT5A 5HNEA 5MR0A 1A3GA 5CE8A
#show ribbon
#hide cartoon
#hide sticks
#bg_color black
#remove solvent
#remove resn Cl
#extra_fit *, 7LV7A, \
#method=cealign, \
#mobile_state=-1, \
#target_state=-1, \
#object=cetest
#save cetest.aln






fetch 7LV7A 4DQNA 3UYYA 6JIFA 3HT5A 5HNEA 5MR0A 1A3GA 5CE8A
color black, 7LV7A
color red, 4DQNA
color blue, 3UYYA
color chocolate, 6JIFA
color forest, 3HT5A
color tv_orange, 5HNEA
color lightteal, 5MR0A
color deepolive, 1A3GA
color br5, 5CE8A

show ribbon
hide cartoon
hide sticks
bg_color black
remove solvent
remove resn Cl
extra_fit *, 7LV7A, \
method=align, \
cycles=5, \
cutoff=2.0, \
mobile_state=-1, \
target_state=-1
bg_color white

set_view (\
    -0.166554123,    0.804610610,   -0.569944978,\
    -0.871826410,    0.149832308,    0.466304660,\
     0.460601807,    0.574569821,    0.676539838,\
     0.000000000,    0.000000000, -279.937835693,\
     9.678412437,  -14.268550873,   11.896087646,\
  -6059.893066406, 6619.769042969,  -20.000000000 )
