# SCRIPT

## INTRO

Vážená skúšobná komisia, vážení prítomní, dobrý deň. Volám sa David
Vavřinek a rád by som vám predstavil svoju bakalársku prácu s názvom
*Classical Knapsack Cryptosystem and its Variations*. Prácu som
vypracoval pod vedením doc. RNDr. Tatiany Jajcayovej, PhD.

## MOTIVÁCIA / KONTEXT

Verejnokľúčová kryptografia vznikla ako odpoveď na problém zdieľania tajného
kľúča pri symetrickej kryptografii. Asymetricka kryptografia vyuziva namiesto
jedného spoločného tajomstva dva kluce - verejný a súkromný. Verejným kľúčom
môže správu zašifrovať ktokoľvek, ale dešifrovať ju má vedieť iba držiteľ
zodpovedajúceho súkromného kľúča. 

Celé to stojí na asymetrii výpočtu: zašifrovanie má byť jednoduché, ale opačný
smer má byť bez súkromného kľúča prakticky nevykonateľný.

Batohové kryptosystémy boli historickým pokusom postaviť túto asymetriu na
probléme súčtu podmnožiny. Pre všeobecný prípad nepoznáme efektívny algoritmus,
preto sa tento problém na prvý pohľad javil ako zaujímavý základ pre
kryptografiu.

V práci sa pozerám na to, ako túto myšlienku využíva Merkle--Hellmanova schéma,
prečo táto konštrukcia zlyhala, aké varianty vznikli a ako sa vybrané varianty
správajú v implementácii.

## SUBSET-SUM A SUPERINCREASING

Problém súčtu podmnožiny je matematicky základ, na ktorom sú tieto
kryptosystémy postavené. Máme danu postupnosť čísel, ktoré si môžeme predstaviť
ako váhy, a máme cieľový súčet. Úlohou je vybrať také váhy, ktorých súčet dá
práve túto hodnotu. Tento problem patri do triedy np-uplnych problemov.

Na to aby sme ho mohli pouzit v kryptosysteme pre dešifrovanie však bude
dôležitý špeciálny prípad, ktorý efektívne riešiť vieme.

Tým je superrastúca postupnosť. V takejto postupnosti je každá ďalšia váha
väčšia než suma vsetkych predchádzajúcich váh dokopy.

Takuto postupnost vieme riesit greedy algoritmus. Postupuje sa od najväčšej
váhy nadol a vždy sa vyberie váha, ktorá sa ešte zmestí do zostávajúceho súčtu.
Je to korektné práve preto, že menšie váhy už nedokážu nahradiť jednu väčšiu
superrastúcu váhu.

## MERKLE--HELLMAN

Merkle--Hellmanov kryptosystem využíva tento rozdiel medzi všeobecným problémom
súčtu podmnožiny a superrastúcou postupnosťou.

Schema zacina generovanim klucov. Najprv vygeneruje súkromná superrastúca
postupnosť. Následne sa zvolí modulo m, ktorý je väčší ako súčet súkromných
váh a násobiteľ r, ktorý je s m nesúdeliteľný. Táto podmienka je
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
sa späť k súčtu nad pôvodnou superrastúcou postupnosťou. Potom už použijeme
greedy algoritmus pre superrastúcu postupnosť a získame pôvodné bity správy.

## PREČO ZLYHAL

Postupne sa ukázalo, že samotná modulárna transformácia v
Merkle--Hellmanovej schéme nestačí na úplné skrytie pôvodnej
superrastúcej štruktúry. Verejný kľúč síce už nevyzerá superrastúco,
ale stále nevznikol ako náhodná všeobecná inštancia. Vznikol
konkrétnym prevodom zo súkromného kľúča, a práve z tohto prevodu môže
unikať využiteľná štruktúra.

Kritickým momentom bol Shamirov útok, ktorý ukázal, že základnú
Merkle--Hellmanovu schému je možné prelomiť v polynomiálnom
čase. Tento útok využíva vzťahy medzi verejnými váhami, ktoré vznikli rovnakým
modulárnym násobiteľom, a z nich hľadá inú transformáciu späť na ľahký prípad.
Dôležité je, že útočník nemusí nájsť presne pôvodný súkromný
kľúč. Stačí mu nájsť ekvivalentný trapdoor, teda inú transformáciu,
ktorá mu umožní previesť verejnú batohovú inštanciu späť na ľahko
riešiteľný prípad.

Druhá línia útokov súvisí s nízkou hustotou. Ak sú verejné váhy veľké vzhľadom
na počet prvkov, subset-sum inštancia má špeciálny tvar, ktorý sa dá využiť
mriežkovými metódami, napríklad pomocou LLL algoritmu.

Hlavné poučenie je teda to, že NP-úplnosť všeobecného problému sama
osebe nezaručuje bezpečnosť konkrétneho kryptosystému. Treba
analyzovať aj to, aké inštancie schéma generuje a akú štruktúru v nich
zanecháva.

## VARIANTY A HISTORICKÝ VÝVOJ

S Merkle--Hellmanovou myšlienkou vznikali aj varianty, ktorých cieľom bolo
lepšie skryť pôvodnú súkromnú štruktúru.

Permutovaný variant pridáva tajnú permutáciu váh, iterovaný variant viac
modulárnych vrstiev. Ani jedna úprava však neodstránila základný problém:
verejný kľúč stále vzniká zo špeciálnej skrytej štruktúry.

V praci spominam aj Chor--Rivest system, ktory nepouziva superrastúcu
postupnosť. je založený na aritmetike v konečných poliach. Aj tu však verejný
kľúč niesol špeciálnu algebraickú štruktúru a navrhované parametre boli
prelomené Vaudenayovým útokom.

Spoločný vzor je teda rovnaký: pokusy o opravu zmenili formu trapdooru alebo
maskovania, ale problém zneužiteľnej štruktúry verejného kľúča zostal.

   ## IMPLEMENTÁCIA

V praktickej časti som pripravil vlastnú C implementáciu vybraných
Merkle--Hellmanových variantov. Cieľom nebola bezpečná kryptografická knižnica,
ale experimentálny nástroj na ukázanie fungovania schém a porovnanie ich
správania.

Na prácu s veľkými celými číslami používam knižnicu GMP. To je dôležité,
pretože s rastúcou veľkosťou bloku rastú aj hodnoty súkromných váh, čísla m a
verejného kľúča.

Implementované sú tri varianty: klasický, permutovaný a iterovaný
Merkle--Hellman. Všetky sú zapojené cez rovnaké rozhranie pre generovanie
kľúčov, šifrovanie a dešifrovanie.

   ## UKÁŽKA CLI

Program sa ovláda cez príkazový riadok a má dva hlavné režimy,  demo a
benchmark. Demo režim ukazuje celý priebeh šifrovania a dešifrovania, benchmark
režim meria keygen, encrypt a decrypt a vypisuje výsledky do CSV.

V demo režime sa volí schéma, veľkosť bloku, vstupná správa a voliteľne seed.
Pri textovom vstupe program správu prevedie na bity, rozdelí ju do blokov
pevnej veľkosti a posledný blok prípadne doplní nulami.

Seed slúži na reprodukovateľnosť. Ak ho nezadám, program použije entropiu
operačného systému. Pri experimentoch som seed zadával explicitne a ďalej sa
rozbalil pre pseudonáhodný generátor.

## EXPERIMENTÁLNE NASTAVENIE

Merania som robil nad release buildom C programu. Python skripty slúžili iba
na spúšťanie programu, zber CSV výstupov a generovanie grafov.

Pri runtime benchmarkoch som porovnával tri varianty pre n od 128 po 4096.
Pre každé nastavenie som použil tri rôzne seedy; z meraných opakovaní sa bral
priemer. Každý beh zároveň overil, že dešifrovaný výstup sedí s pôvodným
vstupom.

   ## VÝSLEDOK: RUNTIME VARIANTOV

Pri meraní celkového času sa ukázalo, že klasický a permutovaný variant majú
veľmi podobné správanie. Permutácia mení poradie prvkov, ale nepridáva zásadne
drahú operáciu.

Iterovaný variant bol podľa očakávania pomalší, pretože používa viac
modulárnych vrstiev. Pri n = 4096 bol klasický variant približne na úrovni
11 ms, permutovaný okolo 12 ms a iterovaný okolo 31 ms.

## VÝSLEDOK: ČO STOJÍ NAJVIAC ČASU

Pri meranii casu jednotlivych faz vidno, že pri veľkých hodnotách n dominuje generovanie
kľúča. Šifrovanie aj dešifrovanie zostávajú v porovnaní s ním veľmi rýchle.

To bola aj jedna z historicky atraktívnych vlastností batohových kryptosystémov:
po vytvorení kľúčov je šifrovanie v podstate len sčítanie vybraných verejných
váh. Problém teda nebol v efektivite, ale v tom, že verejný kľúč stále niesol
zneužiteľnú štruktúru.

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

Hlavný prínos práce je v prehľadnom spracovaní klasických
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
