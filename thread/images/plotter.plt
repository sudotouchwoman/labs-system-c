set terminal gif animate delay 4
set size ratio -1
set output "pendulum.gif"
set xrange [-1:1]
set yrange [-1.5:0.5]
set zrange [-0.5:0.5]
set nokey

set xlabel 'X'
set ylabel 'Y'
set zlabel 'Z'

# unset xtics
# unset ytics
# unset ztics
# unset border

set view 15, 15, 1, 1

splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.479425538604203 -0.8775825618903728 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4780215428037317 -0.8783481112950264 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4738112432235755 -0.880626428058419 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.466800091497177 -0.8843628636358648 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4569984452008731 -0.8894674929889145 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4444233976367903 -0.8958168583103197 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4291011875307727 -0.9032564258612726 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.411070043719281 -0.9116037621448413 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.39038328875162626 -0.9206524251113796 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.3671125020501049 -0.9301765482092696 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.34135052959363993 -0.9399360701378268 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.3132141245325882 -0.9496825323197665 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.28284601279603805 -0.9591653314446803 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.25041620009002624 -0.9681382787249309 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.21612237137479443 -0.9763662840304019 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.18018927968938878 -0.9836319552988401 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.14286707582542332 -0.9897418848594277 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.10442859068998299 -0.9945323873290924 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  6.516564435982236e-2 -0.9978744604383706 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  2.538451549059462e-2 -0.9996777612677537 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -1.4599242492294932e-2 -0.9998934253802508 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -5.446641018931241e-2 -0.9985156033638581 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -9.389988024412708e-2 -0.9955816453160126 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.13259033778420903 -0.991170924879392 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.17024158285333169 -0.9854023561305261 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.20657525765157456 -0.9784307144229404 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.24133477963819308 -0.9704419220834314 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.2742883317339307 -0.9616474983457385 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.30523081908567273 -0.9522783978859802 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.33398476377996117 -0.9425784728938188 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.36040017012922443 -0.9327977901832883 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.38435344967278007 -0.9231860190257615 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.40574554346496705 -0.9139860797399042 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4244994170709183 -0.9054282107966652 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4405571294313594 -0.897724576753472 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4538766898188645 -0.8910645040843397 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.46442891776070894 -0.8856103998641934 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4721945099925054 -0.8814943815662909 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.47716149765074795 -0.8788156263743239 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.47932324770880824 -0.8776384359210121 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4786771269229756 -0.8779910068792081 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4752239060969425 -0.879864898194015 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.46896793905072987 -0.8832151901674421 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4599181059834833 -0.8879613368770993 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4480894666027533 -0.8939887191232673 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4335055261209907 -0.9011509079075286 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.4162009787116262 -0.9092726463055426 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.39622476005888 -0.9181535489863789 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.3736432150517507 -0.9275725027434735 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.34854317024090886 -0.9372927282756529 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.3210346949893786 -0.9470674340368148 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.29125334152958837 -0.9566459590924162 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.2593616730493598 -0.9657802661853456 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.22554992036894558 -0.9742316117954509 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.19003565073796863 -0.9817771903281298 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.15306238479954642 -0.9882165280744274 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -0.11489715692351263 -0.9933773921983999 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -7.582707619422498e-2 -0.9971209828881524 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  -3.615500611251265e-2 -0.9993461940353824 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  3.805463882176116e-3 -0.9999927591961061 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  4.373553683599171e-2 -0.9990431436217695 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  8.331696676262787e-2 -0.996523097097842 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.12223779166032502 -0.992500842463122 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.16019777720348488 -0.98708493665898 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.1969133260708594 -0.9804209004380269 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.23212164232239355 -0.9726867651847406 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.26558398755025286 -0.9640877271062562 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.2970879220015215 -0.9548501278215434 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.3264484852335553 -0.9452149948496994 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.3535083327039177 -0.9354313757346908 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.37813690285309487 -0.9257496868488096 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.40022874013637655 -0.9164152746265466 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.419701140356899 -0.9076623561562519 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4364913137554178 -0.8997084711261029 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4505532778015728 -0.8927495415077281 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.46185469556473024 -0.8869556021497415 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.470373867702006 -0.8824672371159485 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4760970677973522 -0.8793927348089495 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4790163836587343 -0.877805960441434 0
e
splot '-' title 'Pendulum' with linespoints lw 1 pt 7 ps 3 lc rgb "black"
  0 0 0
  0.4791281930259268 -0.8777449371245102 0
e