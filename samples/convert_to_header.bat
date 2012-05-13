..\bin\bin2h amen-bd.pcm amen-bd.hpp amen_bd  > convert_to_header.log
..\bin\bin2h amen-hh.pcm amen-hh.hpp amen_hh >> convert_to_header.log
..\bin\bin2h amen-sn.pcm amen-sn.hpp amen_sn >> convert_to_header.log
type amen-bd.hpp > ..\tinysynth\samples.hpp
type amen-hh.hpp >> ..\tinysynth\samples.hpp
type amen-sn.hpp >> ..\tinysynth\samples.hpp
