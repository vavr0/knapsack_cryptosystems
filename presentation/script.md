# SCRIPT

## INTRO

Vážená skúšobná komisia, vážení prítomní, dobrý deň. Volám sa David
Vavřinek a rád by som vám predstavil svoju bakalársku prácu s názvom
*Classical Knapsack Cryptosystem and its Variations*. Prácu som
vypracoval pod vedením doc. RNDr. Tatiany Jajcayovej, PhD.

## MOTIVÁCIA / KONTEXT

Verejnokľúčová kryptografia vznikla ako odpoveď na problém zdieľania tajného
kľúča pri symetrickej kryptografii. Pri symetrickej kryptografii sa ten istý
tajný kľúč používa na šifrovanie aj dešifrovanie, takže komunikujúce strany si
ho musia bezpečne vymeniť ešte pred komunikáciou.

Asymetricka kryptografia vyuziva namiesto jedného spoločného tajomstva dva
kluce - verejný a súkromný. Verejným kľúčom môže správu zašifrovať ktokoľvek,
ale dešifrovať ju má vedieť iba držiteľ zodpovedajúceho súkromného kľúča. 

Celé to stojí na asymetrii výpočtu: zašifrovanie má byť jednoduché, ale opačný smer
má byť bez súkromného kľúča prakticky nevykonateľný.

Batohové kryptosystémy boli historickým pokusom postaviť túto asymetriu na
probléme súčtu podmnožiny. Jeho rozhodovacia verzia je NP-úplná, a práve preto
sa tento problém na prvý pohľad javil ako silný základ pre kryptografiu.

Merkle--Hellmanov batohovy kryptosystem je dobrý príklad toho, prečo to nestačí. Tento
problém má vyzerať ťažko, ale drzitel súkromneho kľúča ho vie previest na
jednoducho riešiteľnú inštanciu. Ak sa však táto štruktúra dá z verejného kľúča
spätne odhaliť alebo obísť, samotná NP-úplnosť všeobecného problému bezpečnosť
nezaručuje.

## SUBSET-SUM A SUPERINCREASING

Problém súčtu podmnožiny je matematicky základ, na ktorom sú tieto
kryptosystémy postavené. Máme danu postupnosť čísel, ktoré si môžeme predstaviť
ako váhy, a máme cieľový súčet. Úlohou je vybrať také váhy, ktorých súčet dá
práve túto hodnotu. V rozhodovacej verzii sa pýtame iba na to, či taký výber
existuje. 

Vo všeobecnosti nepoznáme efektívny algoritmus, ktorý by tento problém riešil
pre ľubovoľný vstup. Na to aby sme ho mohli pouzit v kryptosysteme pre
dešifrovanie však bude dôležitý špeciálny prípad, ktorý efektívne riešiť vieme.

Tým je superrastúca postupnosť. To znamená, že každý ďalší prvok je väčší ako
súčet všetkých predchádzajúcich prvkov.

Pri takejto postupnosti funguje greedy algoritmus. Postupuje sa od najväčšej
váhy nadol a vždy sa vyberie váha, ktorá sa ešte zmestí do zostávajúceho súčtu.
Je to korektné práve preto, že menšie váhy už nedokážu nahradiť jednu väčšiu
superrastúcu váhu.

## MERKLE--HELLMAN

Merkle--Hellmanova schéma využíva rozdiel medzi všeobecným problémom
súčtu podmnožiny a superrastúcou postupnosťou, ktorá sa dá riešiť
greedy algoritmom.

Schema zacina generovanim klucov. Najprv vytvorí súkromná superrastúca
postupnosť. Následne sa zvolí modulo m, ktorý je väčší ako súčet súkromných
váh. Potom sa zvolí násobiteľ r, ktorý je s m nesúdeliteľný. Táto podmienka je
dôležitá preto, že násobenie číslom r modulo m je potom vratná operácia. Inými
slovami, existuje k nemu modulárna inverzia, ktorú neskôr použijeme pri
dešifrovaní.

Verejný kľúč sa vznikne tak, že každý prvok súkromnej
postupnosti sa vynásobí rovnakým násobiteľom a zoberie sa zvyšok
modulo m. Tak vznikne verejná postupnosť, ktorá už nie je
superrastúca.

Šifrovanie je potom súčet vybraných verejných váh. Bity správy určujú,
ktoré váhy sa do súčtu zahrnú.

Pri dešifrovaní sa šifrový text vynásobí modulárnou inverziou násobiteľa a
zoberie sa zvyšok modulo m. Tým odstranime modularnu transformaciu a dostaneme
sa späť k súčtu nad pôvodnou superrastúcou postupnosťou. Keďže $m$ bolo zvolené
väčšie ako súčet všetkých súkromných váh, tento súčet sa dá jednoznačne
obnoviť. Potom už použijeme greedy algoritmus pre superrastúcu postupnosť a
získame pôvodné bity správy.

Trapdoor je teda v tom, že útočník vidí iba transformovanú subset-sum
inštanciu, zatiaľ čo držiteľ súkromného kľúča ju vie previesť späť na
ľahký superrastúci prípad.

## PREČO ZLYHAL

Postupne sa ukázalo, že samotná modulárna transformácia v
Merkle--Hellmanovej schéme nestačí na úplné skrytie pôvodnej
superrastúcej štruktúry. Verejný kľúč síce už nevyzerá superrastúco,
ale stále nevznikol ako náhodná všeobecná inštancia. Vznikol
konkrétnym prevodom zo súkromného kľúča, a práve z tohto prevodu môže
unikať využiteľná štruktúra.

Kritickým momentom bol Shamirov útok, ktorý ukázal, že základnú
Merkle--Hellmanovu schému je možné prelomiť v polynomiálnom
čase. Dôležité je, že útočník nemusí nájsť presne pôvodný súkromný
kľúč. Stačí mu nájsť ekvivalentný trapdoor, teda inú transformáciu,
ktorá mu umožní previesť verejnú batohovú inštanciu späť na ľahko
riešiteľný prípad.

Ďalšia línia útokov súvisí s nízkou hustotou batohových
inštancií. Hustota zjednodušene vyjadruje pomer medzi počtom prvkov a
veľkosťou čísel vo verejnom kľúči. Pri nízkej hustote sa dajú použiť
mriežkové metódy, napríklad založené na LLL algoritme, ktoré sa snažia
nájsť riešenie subset-sum inštancie priamo.

Hlavné poučenie je teda to, že NP-úplnosť všeobecného problému sama
osebe nezaručuje bezpečnosť konkrétneho kryptosystému. Treba
analyzovať aj to, aké inštancie schéma generuje a akú štruktúru v nich
zanecháva.

## VARIANTY A HISTORICKÝ VÝVOJ

Po objavení slabín základnej Merkle--Hellmanovej schémy vznikali rôzne
varianty, ktorých cieľom bolo lepšie skryť pôvodnú štruktúru
súkromného kľúča.

Jednoduchou úpravou je permutovaný Merkle--Hellman. Ten okrem
modulárnej transformácie používa aj permutáciu prvkov, aby sa ešte
zakrylo poradie pôvodnej superrastúcej postupnosti. Problém je, že
základný vzťah medzi verejnými a súkromnými váhami tým nezmizne.

Ďalším pokusom je iterovaný Merkle--Hellman, kde sa nepoužije iba
jedna modulárna transformácia, ale viac vrstiev za sebou. Myšlienka
je, že opakovaním transformácie by malo byť ťažšie spätne odhaliť
pôvodnú súkromnú štruktúru. Ukázalo sa však, že ani toto nestačí.
Brickellov útok využil mriežkové metódy na obnovenie použiteľnej
skrytej štruktúry aj pri iterovaných batohoch.

V práci spomínam aj Chor--Rivestov kryptosystém. Ten už nie je iba
ďalšou úpravou Merkle--Hellmana, pretože nepoužíva superrastúcu
postupnosť. Trapdoor je postavený na aritmetike v konečných poliach a
verejný kľúč má stále podobu batohovej inštancie. Aj tu sa však neskôr
ukázalo, že verejný kľúč nesie špeciálnu algebraickú štruktúru;
navrhované parametre boli prelomené Vaudenayovým útokom.

Spoločný vzor je teda rovnaký: varianty sa snažili zachovať efektivitu
batohového šifrovania a lepšie zakryť súkromnú štruktúru, ale v praxi
sa ukázalo, že táto štruktúra sa často dá z verejného kľúča stále
využiť. Preto majú tieto systémy dnes skôr historickú a didaktickú
hodnotu než praktické použitie.

## IMPLEMENTÁCIA

V praktickej casti som pripravil vlastnú implementáciu vybraných
Merkle--Hellmanových variantov. Cieľom nebolo vytvoriť bezpečnú
kryptografickú knižnicu, ale experimentálny nástroj, na ktorom sa dá
ukázať fungovanie schém a porovnať ich správanie.

Implementácia je napísaná v jazyku C a používa knižnicu GMP na prácu s
veľkými celými číslami. To je dôležité, pretože už pri väčších
blokových veľkostiach rýchlo rastú hodnoty súkromných váh, modulu aj
verejného kľúča.

Implementované sú tri varianty: klasický Merkle--Hellman, permutovaný
Merkle--Hellman a iterovaný Merkle--Hellman. Všetky sú v kóde zapojené
cez rovnaké rozhranie, ktoré zjednocuje generovanie kľúčov, šifrovanie
a dešifrovanie.

Okrem samotných schém program obsahuje aj demo a benchmark režim. Demo slúži
na kontrolu celého priebehu šifrovania a dešifrovania, benchmark na meranie
jednotlivých operácií.

## UKÁŽKA CLI

Toto je príklad demo režimu. Cez príkazový riadok sa volí schéma, veľkosť
bloku, vstupná správa a voliteľne seed. Pri textovom vstupe program správu
prevedie na bity, rozdelí ju do blokov pevnej veľkosti a posledný blok prípadne
doplní nulami.

Seed slúži na reprodukovateľnosť. Ak ho nezadám, program použije entropiu
operačného systému. Pri experimentoch som seed zadával explicitne a ďalej sa
rozbalil pre PCG generátor.

Benchmark režim používa rovnaké schémy, ale namiesto ukážky správy meria
keygen, encrypt a decrypt a vypisuje výsledky do CSV.

## EXPERIMENTÁLNE NASTAVENIE

Merania som robil nad release buildom C programu. Python skripty slúžili iba
na spúšťanie programu, zber CSV výstupov a generovanie grafov.

Pri runtime benchmarkoch som porovnával tri varianty pre n od 128 po 4096.
Pre každé nastavenie som použil tri rôzne seedy; z meraných opakovaní sa bral
priemer. Každý beh zároveň overil, že dešifrovaný výstup sedí s pôvodným
vstupom.

## VÝSLEDKY: RUNTIME

V experimentoch na cas behu programu sa ukázalo, že klasický a permutovaný variant
majú veľmi podobné správanie. To dáva zmysel, pretože permutácia mení
poradie prvkov, ale nepridáva zásadne drahú operáciu pri samotnom
šifrovaní alebo dešifrovaní.

Iterovaný variant bol pomalší, pretože pri generovaní kľúča aj pri
dešifrovaní pracuje s viacerými modulárnymi transformáciami. Pri
väčších blokových veľkostiach sa tento rozdiel prejaví výraznejšie.

Najdôležitejší výsledok bol, že pri veľkých hodnotách $n$ dominuje
hlavne generovanie kľúča. Šifrovanie a dešifrovanie ostávajú v
porovnaní s ním relatívne rýchle. Napríklad pri $n = 4096$ bol
klasický variant rádovo v desiatkach milisekúnd a iterovaný variant
bol podľa očakávania pomalší.

Tento výsledok zodpovedá štruktúre schémy. Šifrovanie je v podstate
lineárny prechod cez verejné váhy a ich sčítanie podľa bitov
správy. Dešifrovanie pridáva modulárnu inverziu a greedy postup, ale
stále zostáva relatívne jednoduché. Najviac práce vzniká pri
generovaní veľkých parametrov a verejného kľúča.

## VÝSLEDKY: HUSTOTA PRI ITEROVANÍ

Hustotu verejnej batohovej inštancie počítam ako pomer počtu prvkov (n) k
bitovej veľkosti najväčšej verejnej váhy. Pri rovnakom (n) teda nižšia hustota
znamená, že najväčšia verejná váha potrebuje viac bitov.

Pri iterovanom variante každá ďalšia vrstva znova transformuje aktuálne váhy
pomocou nového modulu a násobiteľa. Počet bitov správy zostáva rovnaký, ale
bitová veľkosť verejných váh sa zväčšuje. Preto s počtom vrstiev hustota klesá.

Na grafe je to najvýraznejšie pri menších blokoch. Pri \(n = 128\) klesla
hustota približne z 0.916 pri dvoch vrstvách na 0.812 pri piatich vrstvách.

## VÝSLEDKY: VPLYV PARAMETRA DELTA

Tento graf ukazuje vplyv parametra `delta_max`. V implementácii sa nová
súkromná superrastúca váha generuje ako súčet predchádzajúcich váh plus náhodný
prírastok delta.

Väčšie `delta_max` dáva generátoru viac priestoru zväčšiť súkromné váhy. Tým
rastie súčet súkromných váh, následne modul aj bitová veľkosť verejných váh.
Výsledkom je nižšia hustota.

Pri \(n = 128\) klesla hustota z 1.000 pre `delta_max = 1` na približne 0.948
pre `delta_max = 128`.

Margin som testoval samostatne. Ten sa pridáva až po vytvorení súkromnej
postupnosti ako rezerva medzi súčtom súkromných váh a modulom. V mojich
meraniach mal na hustotu výrazne menší vplyv než delta.

## HLAVNÉ PRÍNOSY PRÁCE

Hlavný prínos práce vidím najmä v prehľadnom spracovaní klasických
batohových kryptosystémov. V práci som sa snažil nielen opísať
Merkle--Hellmanovu schému, ale aj ukázať, prečo zlyhala, aké varianty
vznikli a aké typy útokov sa pri nich objavili.

Druhá časť prínosu je praktická. Implementoval som tri
Merkle--Hellmanove varianty v jazyku C, doplnil ich o demo a benchmark
režim a pripravil reprodukovateľné experimenty.

Práca tak prepája viac oblastí, ktoré som počas štúdia používal:
diskrétnu matematiku, modulárnu aritmetiku, výpočtovú zložitosť,
algoritmy pre subset-sum a praktickú implementáciu s veľkými číslami.

## ZÁVER

Klasické batohové kryptosystémy dnes nie sú vhodné na praktické
šifrovanie. Ich význam je skôr historický a didaktický.

Problém súčtu podmnožiny tým ale z kryptografie úplne nezmizol. Stále
sa objavuje v teoretickom výskume, už však skôr iným spôsobom než ako
skrytá superrastúca štruktúra v štýle Merkle--Hellmana.

Praktická verejná kryptografia sa historicky vydala skôr inými smermi,
napríklad cez RSA alebo eliptické krivky. Aj RSA je dobrý kontrast,
pretože nestojí na NP-úplnom probléme, ale na predpokladanej
náročnosti faktorizácie veľkých čísel.

Ďakujem za pozornosť.
