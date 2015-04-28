/****************************************************************************
**
** Copyright (C) 2001 Hubert de Fraysseix, Patrice Ossona de Mendez.
** All rights reserved.
** This file is part of the PIGALE Toolkit.
**
** This file may be distributed under the terms of the GNU Public License
** appearing in the file LICENSE.HTML included in the packaging of this file.
**
*****************************************************************************/

/*!
\file Action_def.h
\brief Actions

To each QAction is associated a unique int identifier
The header Action.h is automatically generated from this file
The comments defines the macronames which should not include whitespaces
*/


#ifndef  ACTIONS_H_INCLUDED 
#define  ACTIONS_H_INCLUDED  


#define A_PAUSE                    (99)                 // PAUSE

//Augment
#define  A_AUGMENT                  (100)
#define  A_AUGMENT_CONNECT           (A_AUGMENT+1)       // CONNECT
#define  A_AUGMENT_CONNECT_V         (A_AUGMENT+2)       // CONNECT_V
#define  A_AUGMENT_BICONNECT         (A_AUGMENT+3)       // BICONNECT
#define  A_AUGMENT_BICONNECT_6       (A_AUGMENT+4)       // BICONNECT_6
#define  A_AUGMENT_BICONNECT_NP      (A_AUGMENT+5)       // BICONNECT_NP
#define  A_AUGMENT_BICONNECT_NP_V    (A_AUGMENT+6)       // BICONNECT_NP_V
#define  A_AUGMENT_TRIANGULATE_V     (A_AUGMENT+7)       // TRIANGULATE_V
#define  A_AUGMENT_TRIANGULATE_ZZ    (A_AUGMENT+8)       // TRIANGULATE_ZZ
#define  A_AUGMENT_TRIANGULATE_3C    (A_AUGMENT+9)       // TRIANGULATE_3C
#define  A_AUGMENT_QUADRANGULATE_V   (A_AUGMENT+10)      // QUADRANGULATE_V
#define  A_AUGMENT_BISSECT_ALL_E     (A_AUGMENT+11)      // BISSECT_ALL_E
#define  A_AUGMENT_END               (A_AUGMENT+99)

//Embed
#define  A_EMBED                    (200)
#define  A_EMBED_SCHNYDER_E         (A_EMBED+1)         // SCHNYDER_E
#define  A_EMBED_SCHNYDER_V         (A_EMBED+2)         // SCHNYDER_V 
#define  A_EMBED_FPP                (A_EMBED+3)         // FPP_FARY
#define  A_EMBED_TUTTE_CIRCLE       (A_EMBED+4)         // TUTTE_CIRCLE
#define  A_EMBED_TUTTE              (A_EMBED+5)         // TUTTE
#define  A_EMBED_SPRING             (A_EMBED+6)         // SPRING
#define  A_EMBED_SPRING_PM          (A_EMBED+7)         // SPRING_PM
#define  A_EMBED_JACQUARD           (A_EMBED+8)         // JACQUARD 
#define  A_EMBED_POLYLINE           (A_EMBED+9)         // POLYLINE
#define  A_EMBED_CURVES             (A_EMBED+10)        // CURVES  
#define  A_EMBED_CCD                (A_EMBED+11)        // CCD
#define  A_EMBED_CD                 (A_EMBED+12)        // CD
#define  A_EMBED_POLREC_DFS         (A_EMBED+13)        // POLREC_DFS
#define  A_EMBED_POLREC_DFSLR       (A_EMBED+14)        // POLREC_DFSLR
#define  A_EMBED_POLREC_BFS         (A_EMBED+15)        // POLREC_BFS
#define  A_EMBED_VISION             (A_EMBED+50)        // VISION
#define  A_EMBED_CONTACT_BIP        (A_EMBED+51)        // CONTACT_BIPARTI 
#define  A_EMBED_BIP_2PAGES         (A_EMBED+52)        // 2_PAGES_BIPARTI 
#define  A_EMBED_POLAR              (A_EMBED+53)        // POLAR 
#define  A_EMBED_FPP_RECTI          (A_EMBED+54)        // FPP_RECTI
#define  A_EMBED_GVISION            (A_EMBED+55)        // GVISION 
#define  A_EMBED_T_CONTACT          (A_EMBED+56)        // T_CONTACT
#define  A_EMBED_TRIANGLE           (A_EMBED+57)        // TRIANGLE_CONTACT
#define  A_EMBED_3dSCHNYDER         (A_EMBED+58)        // EMBED-3d_SCHNYDER
#define  A_EMBED_3d                 (A_EMBED+98)        // EMBED-3d
#define  A_EMBED_END                (A_EMBED+99)

//Graph
#define  A_GRAPH                    (300)
#define  A_GRAPH_DUAL               (A_GRAPH+1)         // DUAL
#define  A_GRAPH_DUAL_G             (A_GRAPH+2)         // DUAL_G
#define  A_GRAPH_ANGLE              (A_GRAPH+3)         // ANGLE
#define  A_GRAPH_ANGLE_G            (A_GRAPH+4)         // ANGLE_G
#define  A_GRAPH_END                (A_GRAPH+99)

//Remove
#define  A_REMOVE                  (400)
#define  A_REMOVE_ISOLATED_V       (A_REMOVE+1)         // REMOVE_ISOLATED_V 
#define  A_REMOVE_LOOPS            (A_REMOVE+2)         // REMOVE_LOOPS
#define  A_REMOVE_MULTIPLE_E       (A_REMOVE+3)         // REMOVE_MULTIPLE_E
#define  A_REMOVE_BRIDGES          (A_REMOVE+4)         // REMOVE_BRIDGES
#define  A_REMOVE_COLOR_V          (A_REMOVE+5)         // REMOVE_COLOR_V
#define  A_REMOVE_COLOR_E          (A_REMOVE+6)         // REMOVE_COLOR_E
#define  A_REMOVE_THICK_E          (A_REMOVE+7)         // REMOVE_THICK_E
#define  A_REMOVE_END              (A_REMOVE+99)

//Generate
#define  A_GENERATE                 (500)
#define  A_GENERATE_GRID            (A_GENERATE+1)      // GEN_GRID
#define  A_GENERATE_COMPLETE        (A_GENERATE+2)      // GEN_COMPLETE
#define  A_GENERATE_COMPLETE_BIP    (A_GENERATE+3)      // GEN_COMPLETE_BIP
#define  A_GENERATE_RANDOM          (A_GENERATE+4)      // GEN_RANDOM
#define  A_GENERATE_P               (A_GENERATE+5)      // GEN_PLANAR
#define  A_GENERATE_P_2C            (A_GENERATE+6)      // GEN_PLANAR_2C 
#define  A_GENERATE_P_3C            (A_GENERATE+7)      // GEN_PLANAR_3C 
#define  A_GENERATE_P_3R_2C         (A_GENERATE+10)     // GEN_PLANAR_3R_2C
#define  A_GENERATE_P_3R_3C         (A_GENERATE+11)     // GEN_PLANAR_3R_3C
#define  A_GENERATE_P_3R_D4C        (A_GENERATE+12)     // GEN_PLANAR_3R_D4C
#define  A_GENERATE_P_4R_C          (A_GENERATE+13)     // GEN_PLANAR_4R_C 
#define  A_GENERATE_P_4R_2C         (A_GENERATE+14)     // GEN_PLANAR_4R_2C
#define  A_GENERATE_P_4R_3C         (A_GENERATE+15)     // GEN_PLANAR_4R_3C
#define  A_GENERATE_P_4R_BIP        (A_GENERATE+16)     // GEN_PLANAR_4R_BIP 
#define  A_GENERATE_P_BIP           (A_GENERATE+17)     // GEN_PLANAR_BIP
#define  A_GENERATE_P_BIP_2C        (A_GENERATE+18)     // GEN_PLANAR_BIP_2C
#define  A_GENERATE_P_BIP_3C        (A_GENERATE+19)     // GEN_PLANAR_BIP_3C
#define  A_GENERATE_P_OUTER_N       (A_GENERATE+20)     // GEN_P_OUTER_N
#define  A_GENERATE_P_OUTER_NM      (A_GENERATE+21)     // GEN_P_OUTER_NM

#define  A_GENERATE_END             (A_GENERATE+99)

//Algo
#define  A_ALGO                    (600)
#define  A_ALGO_KURATOWSKI         (A_ALGO+1)           // KURATOWSKI
#define  A_ALGO_COTREE_CRITICAL    (A_ALGO+2)           // COTREE_CRITICAL
#define  A_ALGO_NPSET              (A_ALGO+3)           // NPSET
#define  A_ALGO_MAX_PLANAR         (A_ALGO+4)           // FIND_MAX_PLANAR
#define  A_ALGO_RESET_COLORS       (A_ALGO+5)           // RESET_COLORS
#define  A_ALGO_NETCUT             (A_ALGO+6)           // NETCUT 
#define  A_ALGO_GEOMETRIC_CIR      (A_ALGO+7)           // USE_GEOMETRIC_CIR
#define  A_ALGO_LRALGO_CIR         (A_ALGO+8)           // USE_LRALGO_CIR
#define  A_ALGO_SYM                (A_ALGO+9)           // SYMMETRY
#define  A_ALGO_COLOR_BIPARTI      (A_ALGO+10)          // COLOR_BIPARTI
#define  A_ALGO_COLOR_EXT          (A_ALGO+11)          // COLOR_EXT_FACE
#define  A_ALGO_COLOR_NON_CRITIC   (A_ALGO+12)          // COLOR_NON_CRITIC
#define  A_ALGO_COLOR_CONNECTED    (A_ALGO+13)          // COLOR_CONNECTED
#define  A_ALGO_END                (A_ALGO+99)

//Orient
#define  A_ORIENT                  (700)
#define  A_ORIENT_E                (A_ORIENT+1)         // ORIENT_EDGES
#define  A_ORIENT_SHOW             (A_ORIENT+2)         // ORIENT_SHOW_POLES
#define  A_ORIENT_INF              (A_ORIENT+3)         // ORIENT_INF
#define  A_ORIENT_TRICON           (A_ORIENT+4)         // ORIENT_TRICON
#define  A_ORIENT_BIPAR            (A_ORIENT+5)         // ORIENT_BIPARTITE
#define  A_ORIENT_SCHNYDER         (A_ORIENT+6)         // ORIENT_SCHNYDER 
#define  A_ORIENT_BIPOLAR          (A_ORIENT+7)         // ORIENT_BIPOLAR
#define  A_ORIENT_BIPOLAR_NP       (A_ORIENT+8)         // ORIENT_BIPOLAR_NON_PLANAR
#define  A_REORIENT_COLOR          (A_ORIENT+9)         // REORIENT_COLOR_EDGES
#define  A_ORIENT_BFS              (A_ORIENT+10)        // ORIENT_BFS
#define  A_ORIENT_NOE              (A_ORIENT+11)        // UNORIENT_EDGES
#define  A_ORIENT_END              (A_ORIENT+99)

//User Test
#define  A_TEST                    (800)
#define  A_TEST_1                  (A_TEST+1)           // TEST_1 
#define  A_TEST_2                  (A_TEST+2)           // TEST_2 
#define  A_TEST_3                  (A_TEST+3)           // TEST_3 
#define  A_TEST_END                (A_TEST+99)

//Properties
#define  A_PROP_DEFAULT            (1000)
#define  A_PROP_DEF_SHORT          (A_PROP_DEFAULT+1)   // PROP_DEF_SHORT
#define  A_PROP_DEF_INT            (A_PROP_DEFAULT+2)   // PROP_DEF_INT
#define  A_PROP_DEFAULT_END        (A_PROP_DEFAULT+999)

//Settings
#define A_SET                       (10000)
#define A_SET_DOC                   (A_SET+9)
#define A_SET_COLOR                 (A_SET+10)
#define A_SET_DEBUG                 (A_SET+1)
#define A_SET_UNDO                  (A_SET+5)
#define A_SET_RANDOM_SEED           (A_SET+7)
#define A_SET_RANDOM_SEED_CHANGE    (A_SET+8)
#define A_SET_SCH_RECT              (A_SET+2)
#define A_SET_SCH_COLOR             (A_SET+4)
#define A_SET_LFACE                 (A_SET+3)
#define A_SET_ORIENT                (A_SET+20)
#define A_SET_ARROW                 (A_SET+21)
#define A_SET_EXTBRIN               (A_SET+22)
#define A_SET_ERASE_MULT            (A_SET+6)
#define A_SET_GEN_CIR               (A_SET+11)
#define A_SET_PAUSE_DELAY           (A_SET+14)
#define A_SET_SAVE                  (A_SET+13)


// Defines used by the server

#define A_SERVER               (2000)

#define A_SERVER_INIT          (A_SERVER)     
#define A_INFO                 (A_SERVER+1)     
#define A_INFO_N               (A_INFO+1)     // N
#define A_INFO_M               (A_INFO+2)     // M
#define A_INFO_SIMPLE          (A_INFO+3)     // SIMPLE
#define A_INFO_PLANAR          (A_INFO+4)     // PLAN
#define A_INFO_OUTER_PLANAR    (A_INFO+5)     // OPLAN 
#define A_INFO_SERIE_PAR       (A_INFO+6)     // SR
#define A_INFO_MAX_PLANAR      (A_INFO+7)     // MPLAN
#define A_INFO_BIPAR           (A_INFO+8)     // BI
#define A_INFO_MAX_BIPAR       (A_INFO+9)     // MBI
#define A_INFO_REGULIER        (A_INFO+10)    // REG
#define A_INFO_CON1            (A_INFO+11)    // C1
#define A_INFO_CON2            (A_INFO+12)    // C2
#define A_INFO_CON3            (A_INFO+13)    // C3
#define A_INFO_MIN_D           (A_INFO+14)    // MIN_D 
#define A_INFO_MAX_D           (A_INFO+15)    // MAX_D
#define A_INFO_COORD           (A_INFO+16)    // COORD
#define A_INFO_VLABEL          (A_INFO+17)    // VLAB
#define A_INFO_END             (A_INFO+99)     

#define A_INPUT                (A_SERVER+100)    
#define A_INPUT_READ_GRAPH     (A_INPUT+1)    //  RS_GRAPH 
#define A_INPUT_NEW_GRAPH      (A_INPUT+2)    //  N_GRAPH
#define A_INPUT_NEW_VERTEX     (A_INPUT+3)    //  N_V
#define A_INPUT_NEW_EDGE       (A_INPUT+4)    //  N_E
#define A_INPUT_END            (A_INPUT+99)    

#define A_TRANS                (A_SERVER+200)  
#define A_TRANS_SEND_PNG       (A_TRANS+1)    // PNG  
#define A_TRANS_GET_CGRAPH     (A_TRANS+2)    // RC_GRAPH  
#define A_TRANS_SEND_GRAPH_SAVE (A_TRANS+3)    // RS_SAVE_GRAPH  
#define A_TRANS_SEND_PS        (A_TRANS+4)    // PS
#define A_TRANS_END            (A_TRANS+99)  

#define A_SET_GEN              (A_SERVER+300)
#define A_SET_GEN_N1           (A_SET_GEN+1)    // GEN_N1  
#define A_SET_GEN_N2           (A_SET_GEN+2)    // GEN_N2
#define A_SET_GEN_M            (A_SET_GEN+3)    // GEN_M  
#define A_SET_GEN_SEED         (A_SET_GEN+4)    // GEN_SEED
#define A_SET_GEN_END          (A_SET_GEN+99)

#define SERVER_DEBUG           (A_SERVER+999)  // S_DEBUG  


// server separators
const char ACTION_SEP = ':';  //to separate actions
const char PARAM_SEP  = ';';  //to separate parameters
const char TYPE_SEP   = ',';  //to separate the parameter from its type if any

#endif
