/********************** DESCRIPTION **********************
This library is used to generate various range of freuency 
on OC0 pin (PB3, PIN: 4) with Timer0.
---------------------------------------------------------*/


/*********************** DEPENDENCY ***********************/
#include "timer0.h"
/*--------------------------------------------------------*/


/********************* USER FUNCTIONS *********************/ 
// Initializes Tiner0 inn free running Frequency generation mode
void T0freqInit() {
	// put Timer0 into CTC mode
	T0operationMode(T0_OPMODE_CTC);
	// make OC0 toggle on compare match
	T0ocMode(T0_OC0_TOGGLE);
	// make OC0 output
	DDRB |= (1<<PB3);
}

// Starts the frequency generation. Specify the value of
// prescaller and OCR0 from the lookup table bellow for a  
// perticular frequency
void T0freqStart(uint16_t prescaller, uint8_t ocr0Value) {
	T0ClockSelect(prescaller);
	OCR0 = ocr0Value;
} 
/*--------------------------------------------------------*/



/******************* EXAMPLE CODE *******************
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "mega16/int/timer0/T0FreqGen.h"


int main() {
   
	sei();

   T0freqInit();
   T0freqStart(1024, 251); // generates 372Hz


   while(1) {
      
   }
}


---------------------------------------------------*/




/****************** FREQUENCY VALUE BASED ON PRESCALLER AND OCR0 VALUES ******************/
/******************** THIS DATA SET ASSUMES THE MCU IS RUNNING AT 16Mhz *******************
Frq	Presc.  OCR0
31	1024	251
32	1024	243
33	1024	236
34	1024	229
35	1024	222
36	1024	216
37	1024	210
38	1024	205
39	1024	199
40	1024	194
41	1024	190
42	1024	185
43	1024	181
44	1024	177
45	1024	173
46	1024	169
47	1024	165
48	1024	162
49	1024	158
50	1024	155
51	1024	152
52	1024	149
53	1024	146
54	1024	144
55	1024	141
56	1024	139
57	1024	136
58	1024	134
59	1024	131
60	1024	129
61	1024	127
62	1024	125
63	1024	123
64	1024	121
65	1024	119
66	1024	117
67	1024	116
68	1024	114
69	1024	112
70	1024	111
71	1024	109
72	1024	108
73	1024	106
74	1024	105
75	1024	103
76	1024	102
77	1024	100
78	1024	99
79	1024	98
80	1024	97
81	1024	95
82	1024	94
83	1024	93
84	1024	92
85	1024	91
86	1024	90
87	1024	89
88	1024	88
89	1024	87
90	1024	86
91	1024	85
92	1024	84
93	1024	83
94	1024	82
95	1024	81
96	1024	80
98	1024	79
99	1024	78
100	1024	77
101	1024	76
103	1024	75
104	1024	74
106	1024	73
107	1024	72
109	1024	71
110	1024	70
112	1024	69
113	1024	68
115	1024	67
117	1024	66
118	1024	65
120	1024	64
122	1024	63
123	256	253
124	1024	62
125	256	249
126	1024	61
127	256	245
128	1024	60
129	256	241
130	1024	59
131	256	238
132	256	236
133	256	234
134	256	232
135	256	230
136	256	229
137	1024	56
138	256	225
139	256	224
140	256	222
141	256	221
142	1024	54
143	256	218
144	256	216
145	1024	53
146	256	213
147	256	212
148	256	210
149	256	209
150	1024	51
151	256	206
152	256	205
153	1024	50
154	256	202
155	256	201
156	1024	49
157	256	198
158	256	197
159	256	196
160	256	194
161	256	193
162	256	192
163	1024 47
164	256	190
165	256	188
166	1024	46
167	256	186
168	256	185
169	256	184
170	1024	45
171	256	182
172	256	181
173	256	180
174	1024	44
175	256	178
176	256	177
177	256	176
178	1024	43
179	256	174
180	256	173
181	256	172
182	1024	42
183	256	170
184	256	169
185	256	168
186	1024	41
187	256	166
188	256	165
189	256	164
191	1024	40
192	256	162
193	256	161
194	256	160
195	1024	39
197	256	158
198	256	157
199	256	156
200	1024	38
202	256	154
203	256	153
204	256	152
206	1024	37
207	256	150
208	256	149
210	256	148
211	1024	36
213	256	146
214	256	145
216	256	144
217	1024	35
219	256	142
220	256	141
222	256	140
223	1024	34
225	256	138
226	256	137
228	256	136
230	1024	33
231	256	134
233	256	133
235	256	132
237	1024	32
239	256	130
240	256	129
242	256	128
244	1024	31
246	256	126
248	256	125
250	256	124
252	1024	30
254	256	122
256	256	121
258	256	120
260	1024	29
263	256	118
265	256	117
267	256	116
269	1024	28
272	256	114
274	256	113
277	256	112
279	1024	27
282	256	110
284	256	109
287	256	108
289	1024	26
292	256	106
295	256	105
298	256	104
300	1024	25
303	256	102
306	256	101
309	256	100
313	1024	24
316	256	98
319	256	97
322	256	96
326	1024	23
329	256	94
332	256	93
336	256	92
340	1024	22
343	256	90
347	256	89
351	256	88
355	1024	21
359	256	86
363	256	85
368	256	84
372	1024	20
377	256	82
381	256	81
386	256	80
391	1024	19
396	256	78
401	256	77
406	256	76
411	1024	18
417	256	74
422	256	73
428	256	72
434	1024	17
440	256	70
446	256	69
453	256	68
460	1024	16
466	256	66
473	256	65
481	256	64
488	1024	15
490	64	254
492	64	253
494	64	252
496	256	62
498	64	250
500	64	249
502	64	248
504	256	61
506	64	246
508	64	245
510	64	244
512	256	60
514	64	242
517	64	241
519	64	240
521	1024	14
523	64	238
525	64	237
527	64	236
530	256	58
532	64	234
534	64	233
536	64	232
539	256	57
541	64	230
543	64	229
546	64	228
548	256	56
551	64	226
553	64	225
556	64	224
558	1024	13
561	64	222
563	64	221
566	64	220
568	256	54
571	64	218
573	64	217
576	64	216
579	256	53
581	64	214
584	64	213
587	64	212
590	256	52
592	64	210
595	64	209
598	64	208
601	1024	12
604	64	206
607	64	205
610	64	204
613	256	50
616	64	202
619	64	201
622	64	200
625	256	49
628	64	198
631	64	197
635	64	196
638	256	48
641	64	194
644	64	193
648	64	192
651	1024	11
654	64	190
658	64	189
661	64	188
665	256	46
668	64	186
672	64	185
676	64	184
679	256	45
683	64	182
687	64	181
691	64	180
694	256	44
698	64	178
702	64	177
706	64	176
710	1024	10
714	64	174
718	64	173
723	64	172
727	256	42
731	64	170
735	64	169
740	64	168
744	256	41
749	64	166
753	64	165
758	64	164
762	256	40
767	64	162
772	64	161
776	64	160
781	1024	9
786	64	158
791	64	157
796	64	156
801	256	38
806	64	154
812	64	153
817	64	152
822	256	37
828	64	150
833	64	149
839	64	148
845	256	36
850	64	146
856	64	145
862	64	144
868	1024	8
874	64	142
880	64	141
887	64	140
893	256	34
899	64	138
906	64	137
912	64	136
919	256	33
926	64	134
933	64	133
940	64	132
947	256	32
954	64	130
962	64	129
969	64	128
977	1024	7
984	64	126
992	64	125
1000	64	124
1008	256	30
1016	64	122
1025	64	121
1033	64	120
1042	256	29
1050	64	118
1059	64	117
1068	64	116
1078	256	28
1087	64	114
1096	64	113
1106	64	112
1116	1024	6
1126	64	110
1136	64	109
1147	64	108
1157	256	26
1168	64	106
1179	64	105
1190	64	104
1202	256	25
1214	64	102
1225	64	101
1238	64	100
1250	256	24
1263	64	98
1276	64	97
1289	64	96
1302	1024	5
1316	64	94
1330	64	93
1344	64	92
1359	256	22
1374	64	90
1389	64	89
1404	64	88
1420	256	21
1437	64	86
1453	64	85
1471	64	84
1488	256	20
1506	64	82
1524	64	81
1543	64	80
1563	1024	4
1582	64	78
1603	64	77
1623	64	76
1645	256	18
1667	64	74
1689	64	73
1712	64	72
1736	256	17
1761	64	70
1786	64	69
1812	64	68
1838	256	16
1866	64	66
1894	64	65
1923	64	64
1953	1024	3
1984	64	62
2016	64	61
2049	64	60
2083	256	14
2119	64	58
2155	64	57
2193	64	56
2232	256	13
2273	64	54
2315	64	53
2358	64	52
2404	256	12
2451	64	50
2500	64	49
2551	64	48
2604	1024	2
2660	64	46
2717	64	45
2778	64	44
2841	256	10
2907	64	42
2976	64	41
3049	64	40
3125	256	9
3205	64	38
3289	64	37
3378	64	36
3472	256	8
3571	64	34
3676	64	33
3788	64	32
3906	1024	1
3922	8	254
3937	8	253
3953	8	252
3968	8	251
3984	8	250
4000	8	249
4016	8	248
4032	64	30
4049	8	246
4065	8	245
4082	8	244
4098	8	243
4115	8	242
4132	8	241
4149	8	240
4167	64	29
4184	8	238
4202	8	237
4219	8	236
4237	8	235
4255	8	234
4274	8	233
4292	8	232
4310	64	28
4329	8	230
4348	8	229
4367	8	228
4386	8	227
4405	8	226
4425	8	225
4444	8	224
4464	256	6
4484	8	222
4505	8	221
4525	8	220
4545	8	219
4566	8	218
4587	8	217
4608	8	216
4630	64	26
4651	8	214
4673	8	213
4695	8	212
4717	8	211
4739	8	210
4762	8	209
4785	8	208
4808	64	25
4831	8	206
4854	8	205
4878	8	204
4902	8	203
4926	8	202
4950	8	201
4975	8	200
5000	64	24
5025	8	198
5051	8	197
5076	8	196
5102	8	195
5128	8	194
5155	8	193
5181	8	192
5208	256	5
5236	8	190
5263	8	189
5291	8	188
5319	8	187
5348	8	186
5376	8	185
5405	8	184
5435	64	22
5464	8	182
5495	8	181
5525	8	180
5556	8	179
5587	8	178
5618	8	177
5650	8	176
5682	64	21
5714	8	174
5747	8	173
5780	8	172
5814	8	171
5848	8	170
5882	8	169
5917	8	168
5952	64	20
5988	8	166
6024	8	165
6061	8	164
6098	8	163
6135	8	162
6173	8	161
6211	8	160
6250	256	4
6289	8	158
6329	8	157
6369	8	156
6410	8	155
6452	8	154
6494	8	153
6536	8	152
6579	64	18
6623	8	150
6667	8	149
6711	8	148
6757	8	147
6803	8	146
6849	8	145
6897	8	144
6944	64	17
6993	8	142
7042	8	141
7092	8	140
7143	8	139
7194	8	138
7246	8	137
7299	8	136
7353	64	16
7407	8	134
7463	8	133
7519	8	132
7576	8	131
7634	8	130
7692	8	129
7752	8	128
7813	1024	0
7874	8	126
7937	8	125
8000	8	124
8065	8	123
8130	8	122
8197	8	121
8264	8	120
8333	64	14
8403	8	118
8475	8	117
8547	8	116
8621	8	115
8696	8	114
8772	8	113
8850	8	112
8929	64	13
9009	8	110
9091	8	109
9174	8	108
9259	8	107
9346	8	106
9434	8	105
9524	8	104
9615	64	12
9709	8	102
9804	8	101
9901	8	100
10000	8	99
10101	8	98
10204	8	97
10309	8	96
10417	256	2
10526	8	94
10638	8	93
10753	8	92
10870	8	91
10989	8	90
11111	8	89
11236	8	88
11364	64	10
11494	8	86
11628	8	85
11765	8	84
11905	8	83
12048	8	82
12195	8	81
12346	8	80
12500	64	9
12658	8	78
12821	8	77
12987	8	76
13158	8	75
13333	8	74
13514	8	73
13699	8	72
13889	64	8
14085	8	70
14286	8	69
14493	8	68
14706	8	67
14925	8	66
15152	8	65
15385	8	64
15625	256	1
15873	8	62
16129	8	61
16393	8	60
16667	8	59
16949	8	58
17241	8	57
17544	8	56
17857	64	6
18182	8	54
18519	8	53
18868	8	52
19231	8	51
19608	8	50
20000	8	49
20408	8	48
20833	64	5
21277	8	46
21739	8	45
22222	8	44
22727	8	43
23256	8	42
23810	8	41
24390	8	40
25000	64	4
25641	8	38
26316	8	37
27027	8	36
27778	8	35
28571	8	34
29412	8	33
30303	8	32
31250	256	0
31373	1	254
31496	1	253
31621	1	252
31746	1	251
31873	1	250
32000	1	249
32129	1	248
32258	8	30
32389	1	246
32520	1	245
32653	1	244
32787	1	243
32922	1	242
33058	1	241
33195	1	240
33333	8	29
33473	1	238
33613	1	237
33755	1	236
33898	1	235
34043	1	234
34188	1	233
34335	1	232
34483	8	28
34632	1	230
34783	1	229
34934	1	228
35088	1	227
35242	1	226
35398	1	225
35556	1	224
35714	8	27
35874	1	222
36036	1	221
36199	1	220
36364	1	219
36530	1	218
36697	1	217
36866	1	216
37037	8	26
37209	1	214
37383	1	213
37559	1	212
37736	1	211
37915	1	210
38095	1	209
38278	1	208
38462	8	25
38647	1	206
38835	1	205
39024	1	204
39216	1	203
39409	1	202
39604	1	201
39801	1	200
40000	8	24
40201	1	198
40404	1	197
40609	1	196
40816	1	195
41026	1	194
41237	1	193
41451	1	192
41667	64	2
41885	1	190
42105	1	189
42328	1	188
42553	1	187
42781	1	186
43011	1	185
43243	1	184
43478	8	22
43716	1	182
43956	1	181
44199	1	180
44444	1	179
44693	1	178
44944	1	177
45198	1	176
45455	8	21
45714	1	174
45977	1	173
46243	1	172
46512	1	171
46784	1	170
47059	1	169
47337	1	168
47619	8	20
47904	1	166
48193	1	165
48485	1	164
48780	1	163
49080	1	162
49383	1	161
49689	1	160
50000	8	19
50314	1	158
50633	1	157
50955	1	156
51282	1	155
51613	1	154
51948	1	153
52288	1	152
52632	8	18
52980	1	150
53333	1	149
53691	1	148
54054	1	147
54422	1	146
54795	1	145
55172	1	144
55556	8	17
55944	1	142
56338	1	141
56738	1	140
57143	1	139
57554	1	138
57971	1	137
58394	1	136
58824	8	16
59259	1	134
59701	1	133
60150	1	132
60606	1	131
61069	1	130
61538	1	129
62016	1	128
62500	64	1
62992	1	126
63492	1	125
64000	1	124
64516	1	123
65041	1	122
65574	1	121
66116	1	120
66667	8	14
67227	1	118
67797	1	117
68376	1	116
68966	1	115
69565	1	114
70175	1	113
70796	1	112
71429	8	13
72072	1	110
72727	1	109
73394	1	108
74074	1	107
74766	1	106
75472	1	105
76190	1	104
76923	8	12
77670	1	102
78431	1	101
79208	1	100
80000	1	99
80808	1	98
81633	1	97
82474	1	96
83333	8	11
84211	1	94
85106	1	93
86022	1	92
86957	1	91
87912	1	90
88889	1	89
89888	1	88
90909	8	10
91954	1	86
93023	1	85
94118	1	84
95238	1	83
96386	1	82
97561	1	81
98765	1	80
100000	8	9
101266	1	78
102564	1	77
103896	1	76
105263	1	75
106667	1	74
108108	1	73
109589	1	72
111111	8	8
112676	1	70
114286	1	69
115942	1	68
117647	1	67
119403	1	66
121212	1	65
123077	1	64
125000	64	0
126984	1	62
129032	1	61
131148	1	60
133333	1	59
135593	1	58
137931	1	57
140351	1	56
142857	8	6
145455	1	54
148148	1	53
150943	1	52
153846	1	51
156863	1	50
160000	1	49
163265	1	48
166667	8	5
170213	1	46
173913	1	45
177778	1	44
181818	1	43
186047	1	42
190476	1	41
195122	1	40
200000	8	4
205128	1	38
210526	1	37
216216	1	36
222222	1	35
228571	1	34
235294	1	33
242424	1	32
250000	8	3
258065	1	30
266667	1	29
275862	1	28
285714	1	27
296296	1	26
307692	1	25
320000	1	24
333333	8	2
347826	1	22
363636	1	21
380952	1	20
400000	1	19
421053	1	18
444444	1	17
470588	1	16
500000	8	1
533333	1	14
571429	1	13
615385	1	12
666667	1	11
727273	1	10
800000	1	9
888889	1	8
1000000	8	0
1142857	1	6
1333333	1	5
1600000	1	4
2000000	1	3
2666667	1	2
4000000	1	1
8000000	1	0
*/


