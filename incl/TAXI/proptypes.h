#define TYPE_PROP_DRAW_COORD    Tpoint
#define TYPE_PROP_DRAW_POINT_1    Tpoint
#define TYPE_PROP_DRAW_POINT_2    Tpoint
#define TYPE_PROP_DRAW_POINT_3    Tpoint
#define TYPE_PROP_DRAW_POINT_4    Tpoint
#define TYPE_PROP_DRAW_POINT_5    Tpoint
#define TYPE_PROP_DRAW_POINT_6    Tpoint
#define TYPE_PROP_DRAW_DBLE_1    double
#define TYPE_PROP_DRAW_DBLE_2    double
#define TYPE_PROP_DRAW_DBLE_3    double
#define TYPE_PROP_DRAW_DBLE_4    double
#define TYPE_PROP_DRAW_DBLE_5    double
#define TYPE_PROP_DRAW_DBLE_6    double
#define TYPE_PROP_DRAW_DBLE_MIN    double
#define TYPE_PROP_DRAW_DBLE_MAX    double
#define TYPE_PROP_DRAW_INT_1    int
#define TYPE_PROP_DRAW_INT_2    int
#define TYPE_PROP_DRAW_INT_3    int
#define TYPE_PROP_DRAW_INT_4    int
#define TYPE_PROP_DRAW_INT_5    int
#define TYPE_PROP_DRAW_INT_6    int
#define TYPE_PROP_TITRE    tstring
#define TYPE_PROP_CONNECTED    int
#define TYPE_PROP_BICONNECTED    int
#define TYPE_PROP_TRICONNECTED    int
#define TYPE_PROP_FOURCONNECTED    int
#define TYPE_PROP_PLANAR    int
#define TYPE_PROP_PLANARMAP    int
#define TYPE_PROP_N    int
#define TYPE_PROP_M    int
#define TYPE_PROP_EXTBRIN    tbrin
#define TYPE_PROP_SIMPLE    int
#define TYPE_PROP_BIPARTITE    int
#define TYPE_PROP_MAPTYPE    int
#define TYPE_PROP_SYM    int
#define TYPE_PROP_HYPERGRAPH    int
#define TYPE_PROP_NV    int
#define TYPE_PROP_NE    int
#define TYPE_PROP_NCV    int
#define TYPE_PROP_NCE    int
#define TYPE_PROP_NLOOPS    int
#define TYPE_PROP_MN    int
#define TYPE_PROP_MM    int
#define TYPE_PROP_VSLABEL    svector<tstring *>
#define TYPE_PROP_ESLABEL    svector<tstring *>
#define TYPE_PROP_POINT_MIN    Tpoint
#define TYPE_PROP_POINT_MAX    Tpoint
#define TYPE_PROP_RNEMBED    RnEmbeddingPtr
#define TYPE_PROP_LABEL    int
#define TYPE_PROP_COLOR    short
#define TYPE_PROP_MARK    bool
#define TYPE_PROP_TYPE    int
#define TYPE_PROP_FLAGS    long
#define TYPE_PROP_ORDER    int
#define TYPE_PROP_PMETA    tbrin
#define TYPE_PROP_BIRTH    long
#define TYPE_PROP_DEATH    long
#define TYPE_PROP_VALUE    double
#define TYPE_PROP_SLABEL    int
#define TYPE_PROP_INITIALV    tvertex
#define TYPE_PROP_NEW    tvertex
#define TYPE_PROP_CLASSV    tvertex
#define TYPE_PROP_REPRESENTATIVEV    tvertex
#define TYPE_PROP_COORD    Tpoint
#define TYPE_PROP_PBRIN    tbrin
#define TYPE_PROP_STLINK    tvertex
#define TYPE_PROP_LOW    tvertex
#define TYPE_PROP_ELOW    tedge
#define TYPE_PROP_TSTATUS    char
#define TYPE_PROP_DUALORDER    int
#define TYPE_PROP_DEGREE    int
#define TYPE_PROP_SYMLABEL    int
#define TYPE_PROP_REDUCED    tvertex
#define TYPE_PROP_HYPEREDGE    bool
#define TYPE_PROP_NLOOPS    int
#define TYPE_PROP_COORD3    Tpoint3
#define TYPE_PROP_EIGEN    double
#define TYPE_PROP_CANVAS_ITEM    void *
#define TYPE_PROP_CANVAS_COORD    Tpoint
#define TYPE_PROP_INITIALE    tedge
#define TYPE_PROP_CLASSE    tedge
#define TYPE_PROP_REPRESENTATIVEE    tedge
#define TYPE_PROP_COLOR2    short
#define TYPE_PROP_WIDTH    int
#define TYPE_PROP_ORIENTED    bool
#define TYPE_PROP_REORIENTED    bool
#define TYPE_PROP_ISTREE    bool
#define TYPE_PROP_MULTIPLICITY    int
#define TYPE_PROP_CANVAS_ITEM    void *
#define TYPE_PROP_ISTREE_LR    bool
#define TYPE_PROP_INITIALB    tbrin
#define TYPE_PROP_CLASSB    tbrin
#define TYPE_PROP_VIN    tvertex
#define TYPE_PROP_CIR    tbrin
#define TYPE_PROP_ACIR    tbrin
#define GPROP(_gc,_name,_var) Prop1<TYPE_PROP_##_name > _var(_gc.Set(),PROP_##_name)
#define VPROP(_gc,_name,_var) Prop<TYPE_PROP_##_name > _var(_gc.Set(tvertex()),PROP_##_name)
#define EPROP(_gc,_name,_var) Prop<TYPE_PROP_##_name > _var(_gc.Set(tedge()),PROP_##_name)
#define BPROP(_gc,_name,_var) Prop<TYPE_PROP_##_name > _var(_gc.Set(tbrin()),PROP_##_name)
#define GPROPX(_gc,_name,_var,_val) Prop1<TYPE_PROP_##_name > _var(_gc.Set(),PROP_##_name,_val)
#define VPROPX(_gc,_name,_var,_val) Prop<TYPE_PROP_##_name > _var(_gc.Set(tvertex()),PROP_##_name,_val)
#define EPROPX(_gc,_name,_var,_val) Prop<TYPE_PROP_##_name > _var(_gc.Set(tedge()),PROP_##_name,_val)
#define BPROPX(_gc,_name,_var,_val) Prop<TYPE_PROP_##_name > _var(_gc.Set(tbrin()),PROP_##_name,_val)
