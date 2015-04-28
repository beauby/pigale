BEGIN {
print "#include <TAXI/graph.h>"
print "#include <TAXI/Tpoint.h>"
print "#include <TAXI/embedrn.h>"
print "#include <TAXI/proptypes.h>"
print "void StdGC_init(GraphContainer &GC)"
print "  {"
section=0
}
/@{/ { 
section++
if (section==1) { todo[0]=todo[1]=todo[2]=todo[3]=1}
else if (section==2) todo[1]=todo[2]=todo[3]=0;
else if (section==3) {todo[0]=0; todo[1]=todo[2]=todo[3]=1;}
else if (section==4) {todo[2]=todo[3]=0;}
else if (section==5) {todo[1]=0; todo[2]=1;}
else if (section==6) {todo[2]=0; todo[3]=1;}
}

/#define PROP_[^<]*<[^\\]*\\c/ {
if ($6!="" && $6!="anything")
  { name=gensub("^PROP_","",1,$2);
  if (todo[0]) print "{GPROP(GC," name ",x);}";
  if (todo[1]) print "{VPROP(GC," name ",x);}";
  if (todo[2]) print "{EPROP(GC," name ",x);}";
  if (todo[3]) print "{BPROP(GC," name ",x);}";
  }
}
END {
print "}"
}
