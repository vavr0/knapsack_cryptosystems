# SCRIPT

## INTRO

Vážená skúšobná komisia, vážení prítomní, dobrý deň. Volám sa David
Vavřinek a rád by som vám predstavil svoju bakalársku prácu s názvom
*Classical Knapsack Cryptosystem and its Variations*. Prácu som
vypracoval pod vedením doc. RNDr. Tatiany Jajcayovej, PhD.

## MOTIVÁCIA / KONTEXT

### v2

Verejnokľúčová kryptografia vznikla ako riešenie problému zdieľania
tajného kľúča. Namiesto jedného spoločného tajomstva používa verejný a
súkromný kľúč. Matematicky za tým stojí problém, ktorý sa jedným
smerom počíta ľahko, ale opačný smer je bez súkromnej informácie
prakticky ťažký.

Pri RSA je takým problémom faktorizácia veľkého čísla. Pri batohových
kryptosystémoch je to problém súčtu podmnožiny. Rozdiel je v tom, že
pri batohových systémoch sa historicky pracovalo s problémom, ktorého
rozhodovacia verzia je NP-úplná, čo na prvý pohľad vyzeralo ako silný
základ.

Merkle--Hellmanova schéma je dobrý príklad toho, prečo to
nestačí. Verejný problém má vyzerať ťažko, ale súkromný kľúč obsahuje
trapdoor, ktorý ho prevedie na ľahký prípad. Ak sa však táto štruktúra
dá z verejného kľúča spätne odhaliť alebo obísť, samotná NP-úplnosť
všeobecného problému bezpečnosť nezaručuje.

### v1

Verejnokľúčová kryptografia vznikla ako odpoveď na problém zdieľania
tajného kľúča. Namiesto jedného spoločného kľúča používa dvojicu:
verejný kľúč na šifrovanie a súkromný kľúč na dešifrovanie. Tieto
kľúče sú generované kryptografickým algoritmom založeným na
matematických problémoch, ktoré sa správajú ako jednosmerné funkcie.

Jednosmerná funkcia v tomto kontexte neznamená, že spätný smer je
úplne nemožný. Znamená to, že výpočet jedným smerom je jednoduchý, ale
opačný smer je bez dodatočnej tajnej informácie výpočtovo náročný.

Batohové kryptosystémy sú zaujímavé z viacerých dôvodov. Po prvé, na
Merkle--Hellmanovej schéme sa dobre ukazuje základná myšlienka
trapdooru: verejný problém má vyzerať ťažko, ale držiteľ súkromnej
informácie ho vie previesť na ľahký prípad. Po druhé, tieto systémy
boli historicky atraktívne aj preto, že šifrovanie a dešifrovanie
mohli byť veľmi efektívne.

Zároveň však práve na tomto príklade vidno, prečo samotný výber
ťažkého problému nestačí. Problém súčtu podmnožiny je vo všeobecnosti
výpočtovo náročný a jeho rozhodovacia verzia je NP-úplná, ale
konkrétna kryptografická schéma generuje špeciálne inštancie. Ak v
nich zostane štruktúra súkromného kľúča, útočník ju môže využiť.

## SUBSET-SUM A SUPERINCREASING

Problém súčtu podmnožiny je základ, na ktorom sú tieto kryptosystémy
postavené. Máme postupnosť čísel, ktoré si môžeme predstaviť ako váhy,
a máme cieľový súčet. Úloha je zistiť, ktoré z týchto váh treba
vybrať, aby ich súčet dal práve danú hodnotu.

Vo všeobecnosti je rozhodovacia verzia tohto problému NP-úplná. V
kryptografii nás však nezaujíma iba otázka, či také riešenie existuje,
ale aj nájdenie konkrétnej podmnožiny, teda bitov správy.

V kryptosystéme sa to dá prirodzene prepojiť so správou. Správa je
zapísaná ako postupnosť bitov. Bit jedna znamená, že danú váhu do
súčtu zahrniem, bit nula znamená, že ju nezahrniem. Šifrový text je
potom práve výsledný súčet vybraných verejných váh.

Pre dešifrovanie však potrebujeme špeciálny prípad, ktorý je ľahko
riešiteľný. Tým je superrastúca postupnosť. To znamená, že každý ďalší
prvok je väčší ako súčet všetkých predchádzajúcich prvkov.

Pri takejto postupnosti sa riešenie dá nájsť greedy spôsobom: začnem
od najväčšej váhy, a ak sa ešte zmestí do zvyšného súčtu, použijem
ju. Potom pokračujem smerom nadol. Toto je ľahký prípad, ku ktorému
sa Merkle--Hellman pri dešifrovaní potrebuje dostať späť.

## MERKLE--HELLMAN

Merkle--Hellmanova schéma využíva rozdiel medzi všeobecným problémom
súčtu podmnožiny a jeho ľahkým superrastúcim prípadom. Súkromný kľúč
obsahuje superrastúcu postupnosť $w$, teda ľahký prípad problému,
ktorý vieme efektívne riešiť.

Pri generovaní kľúča sa zvolí modul $m$, ktorý je väčší ako súčet
prvkov postupnosti $w$, a násobiteľ $r$, pre ktorý platí
$\gcd(r,m)=1$. Táto podmienka je dôležitá preto, aby existovala
modulárna inverzia $r^{-1}$. Tú vieme vypočítať napríklad rozšíreným
Euklidovým algoritmom.

Verejný kľúč sa potom vytvorí vzťahom

$$
b_i = r w_i \bmod m.
$$

Inými slovami, každý prvok súkromnej postupnosti sa vynásobí rovnakým
násobiteľom modulo $m$. Tak vznikne verejná postupnosť $b$, ktorá už
nie je superrastúca.

Šifrovanie je potom súčet vybraných verejných váh:

$$
c = \sum x_i b_i.
$$

Bity správy určujú, ktoré váhy sa do súčtu zahrnú.

Pri dešifrovaní sa použije modulárna inverzia $r^{-1}$:

$$
c' = r^{-1}c \bmod m.
$$

Tým sa dostaneme späť k súčtu nad pôvodnou superrastúcou
postupnosťou. Keďže $m$ bolo zvolené väčšie ako súčet všetkých
súkromných váh, tento súčet sa dá jednoznačne obnoviť. Potom už
použijeme greedy algoritmus pre superrastúcu postupnosť a získame
pôvodné bity správy.

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

To je hlavný dôvod, prečo tu nestačí argument, že problém súčtu
podmnožiny je vo všeobecnosti náročný a jeho rozhodovacia verzia je
NP-úplná. Kryptosystém nepoužíva ľubovoľné inštancie tohto problému,
ale inštancie generované konkrétnou schémou. A bezpečnosť závisí práve
od toho, či táto schéma nezanecháva vo verejnom kľúči štruktúru.

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

Po teoretickej časti som pripravil vlastnú implementáciu vybraných
Merkle--Hellmanových variantov. Cieľom nebolo vytvoriť bezpečnú
kryptografickú knižnicu, ale experimentálny nástroj, na ktorom sa dá
ukázať fungovanie schém a porovnať ich správanie.

Implementácia je napísaná v jazyku C a používa knižnicu GMP na prácu s
veľkými celými číslami. To je dôležité, pretože už pri väčších
blokových veľkostiach rýchlo rastú hodnoty súkromných váh, modulu aj
verejného kľúča.

Implementované sú tri varianty: klasický Merkle--Hellman, permutovaný
Merkle--Hellman a iterovaný Merkle--Hellman. V kóde majú spoločné
rozhranie pre generovanie kľúčov, šifrovanie a dešifrovanie, takže sa
dajú testovať rovnakým spôsobom.

Pri implementácii som sa snažil oddeliť samotné schémy od režimov
programu. Vďaka tomu rovnaká implementácia môže bežať v demo režime, v
benchmark režime aj v jednoduchom attack režime.

## REŽIMY PROGRAMU

Program má tri hlavné režimy. Demo režim ukazuje celý priebeh na
jednej správe: vygenerovanie kľúčov, zašifrovanie a následné
dešifrovanie.

Benchmark režim slúži na experimentálne porovnanie jednotlivých
variantov. Samostatne meria čas generovania kľúčov, šifrovania a
dešifrovania pre rôzne veľkosti vstupu a rôzne varianty schémy.

Attack režim obsahuje jednoduché solver experimenty pre problém súčtu
podmnožiny. Implementovaný je brute force a meet-in-the-middle
prístup. Tento režim nemá predstavovať reálnu kryptanalýzu
Merkle--Hellmanovej schémy, teda nejde o Shamirov ani LLL útok. Slúži
skôr ako baseline na porovnanie priameho hľadania riešenia subset-sum
problému.

## EXPERIMENTÁLNE NASTAVENIE

Experimenty som rozdelil na dve časti. Prvá časť porovnáva
kryptografické operácie implementovaných variantov, teda generovanie
kľúčov, šifrovanie a dešifrovanie. Druhá časť sa pozerá na parametre,
hlavne na hustotu verejných batohových inštancií a na jednoduché
solver experimenty.

Merania som robil pre blokové veľkosti od $n = 128$ po $n = 4096$. Pri
každom nastavení som použil tri deterministické seedy, aby výsledky
nezáviseli iba od jedného náhodného behu.

Program vie bežať aj bez explicitného seedu; vtedy si seed berie z
entropie operačného systému, napríklad cez `getrandom` alebo
`arc4random_buf` podľa platformy. V experimentoch som však seed zadával
cez CLI. Jeden seed sa v kóde rozbalí na dve 64-bitové hodnoty pre PCG
generátor, takže rovnaké nastavenie reprodukuje rovnaký keygen aj
vstupnú správu.

Pri runtime experimentoch som sledoval samostatne čas generovania
kľúčov, šifrovania a dešifrovania. Pri parametroch som sledoval najmä
veľkosť čísel vo verejnom kľúči a hustotu inštancie.

Tieto experimenty nemajú dokazovať bezpečnosť schém. Keďže klasické
batohové kryptosystémy sú už známe ako prelomené, cieľom bolo skôr
ukázať, ako sa jednotlivé varianty správajú z hľadiska času, veľkosti
parametrov a štruktúry verejných inštancií.

## VÝSLEDKY: RUNTIME

V runtime experimentoch sa ukázalo, že klasický a permutovaný variant
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

## VÝSLEDKY: HUSTOTA A PARAMETRE

Pri tejto časti som nesledoval len to, ako rýchlo program beží, ale aj
aké verejné batohové inštancie implementácia vytvára. Hustotu som
počítal ako pomer počtu prvkov $n$ k bitovej veľkosti najväčšej
verejnej váhy.

Toto je dôležité preto, že hustota priamo súvisí s tvarom verejného
kľúča. Ak verejné váhy rastú rýchlejšie než počet prvkov, batohová
inštancia sa stáva redšou. To ešte samo osebe nie je dôkaz útoku, ale
je to parameter, ktorý sa v kryptanalýze batohových systémov sleduje.

Aby sa dali výsledky interpretovať, treba povedať, čo v mojej
implementácii znamená delta a margin. Nová súkromná váha sa generuje
ako aktuálny súčet predchádzajúcich váh plus náhodný prírastok delta.
Parameter `delta_max` určuje hornú hranicu tohto prírastku; v základnej
konfigurácii je to 64. Margin je naopak rezerva pri voľbe modulu:
modul sa volí ako súčet súkromných váh plus náhodný margin, pričom
základná horná hranica marginu je $64n$.

Pri delta experimentoch sa ukázalo to, čo zodpovedá implementácii. Keď
je delta malá, súkromná superrastúca postupnosť rastie takmer
minimálne a hustota je veľmi blízko jednej. Keď sa horná hranica delty
zväčšuje, zväčšujú sa súkromné váhy, potom modul aj verejné váhy, a
hustota klesá.

Pri margin parametroch bol efekt oveľa menší. Dôvod je, že margin sa v
mojej implementácii pridáva k už existujúcemu súčtu súkromných
váh. Tento súčet rastie veľmi rýchlo, zatiaľ čo margin je viazaný len
lineárne na $n$. Preto zmena margin faktora zmení konkrétny modul, ale
často nezmení bitovú veľkosť verejných váh natoľko, aby sa to výrazne
prejavilo na hustote.

Najzaujímavejší je iterovaný variant. Každá ďalšia vrstva znovu
vytvára modulárnu transformáciu nad aktuálnymi verejnými váhami. Tým
sa zvyšuje veľkosť čísel, ale počet bitov správy zostáva
rovnaký. Preto s počtom vrstiev hustota klesá a zároveň rastie čas
generovania kľúča.

Toto pekne ukazuje kompromis pri historických variantoch. Iterovanie
malo lepšie skryť pôvodný trapdoor, ale v implementácii vidno, že za
to platíme väčšími číslami, pomalším generovaním kľúča a nižšou
hustotou verejnej inštancie.

## JEDNODUCHÉ SOLVER EXPERIMENTY

V poslednej časti experimentov som sa pozrel na priame riešenie
subset-sum problému na malých inštanciách. Tu som porovnal brute force
a meet-in-the-middle prístup.

Brute force prechádza všetky možné bitové vektory, takže jeho počet
možností rastie ako $2^n$. Je jednoduchý, ale veľmi rýchlo sa stáva
nepoužiteľným.

Meet-in-the-middle rozdelí váhy na dve polovice. Pre každú polovicu
vytvorí možné súčty a potom hľadá dvojice, ktoré spolu dávajú cieľový
súčet. Časovo je to výrazne lepšie než brute force, približne na
úrovni $2^{n/2}$, ale platí sa za to pamäťou.

Tieto experimenty treba chápať len ako baseline. Nejde o implementáciu
Shamirovho útoku ani mriežkových metód. Ich cieľom bolo ukázať rozdiel
medzi priamym hľadaním správy a útokmi, ktoré využívajú štruktúru
kryptosystému.

## HLAVNÉ PRÍNOSY PRÁCE

Hlavný prínos práce vidím najmä v prehľadnom spracovaní klasických
batohových kryptosystémov. V práci som sa snažil nielen opísať
Merkle--Hellmanovu schému, ale aj ukázať, prečo zlyhala, aké varianty
vznikli a aké typy útokov sa pri nich objavili.

Druhá časť prínosu je praktická. Implementoval som tri
Merkle--Hellmanove varianty v jazyku C, doplnil ich o demo, benchmark
a jednoduchý solver režim a pripravil reprodukovateľné experimenty.

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
