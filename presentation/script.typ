#set page(paper: "a4")

== INTRO

Vážená štátna skúšobná komisia,vážení prítomní, dobrý deň. Volám sa David Vavřinek a teraz predstavím svoju bakalársku prácu s názvom Classical Knapsack Cryptosystem and its Variations. Prácu som vypracoval pod vedením doc. RNDr. Tatiany Jajcayovej, PhD.

Práca sa venuje klasickým batohovým kryptosystémom a ich variantom. Tieto kryptosystémy vznikli ako skorý pokus o verejnú
kryptografiu založenú na probléme súčtu podmnožiny. V práci opisujem ich základnú konštrukciu, známe slabiny a vybrané
historické modifikácie. Praktická časť obsahuje implementáciu vybraných Merkle–Hellmanových variantov a experimentálne
porovnanie ich správania.

== PREDSTAVENIE TEMY

Na tejto téme ma zaujalo hlavne to, že ide o kryptosystémy, ktoré boli pôvodne motivované ťažkým matematickým problémom,
ale napriek tomu v praxi zlyhali. To je podľa mňa dobrý príklad toho, že pri kryptografii nestačí poznať len všeobecnú
zložitosť problému, ale treba sa pozerať aj na konkrétnu konštrukciu a na štruktúru kľúčov, ktoré generuje.

Preto som sa v práci nesústredil iba na opis schémy, ale aj na jej známe slabiny, varianty a vlastnú implementáciu, na
ktorej sa dá správanie týchto systémov experimentálne ukázať.

#line(length: 100%)


Téma práce sú klasické batohové kryptosystémy, najmä Merkle–Hellmanova schéma. V prezentácii najprv stručne vysvetlím,
prečo je táto téma zaujímavá, potom ukážem základnú myšlienku schémy, jej známe slabiny, moju implementáciu a nakoniec
experimentálne výsledky.


prehladova/obsahova
skory pokus -> jeden z prvych
implementáciu -> roznych rezimov ak tak
konstrukcia/myslieska schemy
