# 11P — Asymptotická výpočtová zložitosť, veľké O, amortizovaná zložitosť

## Ako čítať otázku

Otázka chce tri veci:

1. čo je výpočtová/asymptotická zložitosť,
2. čo znamená notácia veľké O,
3. čo je amortizovaná zložitosť a prečo nie je to isté ako priemerný prípad.

Dobrý záver je prepojiť to s bakalárkou: subset-sum, brute force, meet-in-the-middle, runtime implementácie Merkle--Hellmanových variantov.

---

## 1. Výpočtová zložitosť

Výpočtová zložitosť opisuje, koľko zdrojov potrebuje algoritmus vzhľadom na veľkosť vstupu.

Najčastejšie zdroje:

- **časová zložitosť**: počet elementárnych operácií,
- **pamäťová zložitosť**: množstvo použitej pamäte.

Veľkosť vstupu označujeme typicky `n`.

Príklady:

- pri poli je `n` počet prvkov,
- pri grafe môže byť `n = |V|` a `m = |E|`,
- pri texte môže byť `n` dĺžka textu,
- pri veľkých číslach môže byť dôležitá aj bitová dĺžka čísla.

Elementárne operácie môžu byť napríklad:

- priradenie,
- porovnanie,
- aritmetická operácia,
- prístup do poľa,
- volanie funkcie,
- návrat z funkcie.

Pri teoretickej analýze často rátame operácie abstraktne. Pri implementácii s veľkými číslami však už jedna aritmetická operácia nemusí byť konštantná, ale závisí od počtu bitov čísla.

---

## 2. Typy prípadov

Pri zložitosti treba povedať, o aký prípad ide.

- **Najlepší prípad**: najpriaznivejší vstup.
- **Priemerný prípad**: očakávané správanie pre nejaké rozdelenie vstupov.
- **Najhorší prípad**: garantovaná horná hranica pre každý vstup danej veľkosti.

Na štátniciach sa väčšinou myslí najhorší prípad, ak nie je povedané inak.

Príklad:

- lineárne hľadanie v poli:
  - najlepší prípad `O(1)`, ak je prvok hneď prvý,
  - najhorší prípad `O(n)`, ak je prvok posledný alebo tam nie je.

---

## 3. Asymptotická zložitosť

Asymptotická zložitosť sleduje rast zložitosti pre veľké vstupy.

Nezaujíma nás presný čas v sekundách, konkrétny počítač ani malé konštanty. Zaujíma nás, ako rýchlo rastie počet operácií, keď rastie `n`.

Preto ignorujeme:

- konštantné faktory,
- nižšie členy,
- malé hodnoty vstupu.

Príklad:

`3n^2 + 10n + 5` má asymptoticky zložitosť `O(n^2)`, lebo kvadratický člen dominuje pre veľké `n`.

Výhoda asymptotickej analýzy:

- vieme porovnať algoritmy nezávisle od konkrétneho hardvéru,
- vieme odhadnúť správanie pre veľké vstupy,
- vieme rozlíšiť algoritmy, ktoré sa škálujú dobre a zle.

---

## 4. Notácia veľké O

Veľké O dáva horný asymptotický odhad funkcie.

Formálne:

`f(n) ∈ O(g(n))`, ak existujú konštanty `c > 0` a `n0 > 0` také, že pre všetky `n >= n0` platí:

```text
0 <= f(n) <= c * g(n)
```

Význam:

- od nejakej veľkosti vstupu je `f(n)` najviac konštantný násobok `g(n)`,
- `g(n)` je horná hranica rastu,
- konštanta `c` absorbuje rozdiely typu `3n` vs `n`.

Príklad:

`f(n) = 3n + 7` je `O(n)`, lebo od určitého `n` platí `3n + 7 <= c*n`, napríklad pre vhodné `c`.

Ďalší príklad:

`2n^2 + 100n + 50` je `O(n^2)`.

---

## 5. Pozor na význam veľkého O

Veľké O je horný odhad, nie vždy tesný odhad.

Napríklad:

- `n` je aj `O(n)`,
- ale formálne aj `O(n^2)`,
- lebo `n` rastie pomalšie ako `n^2`.

Keď chceme tesný odhad, používa sa `Theta`:

- `O(g(n))` = horný odhad,
- `Ω(g(n))` = dolný odhad,
- `Θ(g(n))` = tesný odhad zhora aj zdola.

Na otázku stačí vedieť hlavne `O`, ale dobré je spomenúť rozdiel.

---

## 6. Typické triedy zložitosti

Od najlepších po horšie:

```text
O(1) < O(log n) < O(n) < O(n log n) < O(n^2) < O(n^3) < O(2^n) < O(n!)
```

### `O(1)` konštantná

- nezávisí od veľkosti vstupu,
- príklad: prístup na index poľa.

### `O(log n)` logaritmická

- vstup sa v každom kroku výrazne zmenší, typicky delí,
- príklad: binárne vyhľadávanie v utriedenom poli.

### `O(n)` lineárna

- jeden prechod cez vstup,
- príklad: nájdenie maxima v poli.

### `O(n log n)`

- typické pre efektívne porovnávacie triedenie,
- príklad: merge sort, heapsort, quicksort v priemere.

### `O(n^2)` kvadratická

- dve vnorené slučky cez `n`,
- príklad: bubble sort, insertion sort v najhoršom prípade.

### `O(2^n)` exponenciálna

- prehľadávanie všetkých podmnožín,
- príklad: brute force subset-sum.

### `O(n!)` faktoriálová

- prehľadávanie všetkých permutácií,
- príklad: bruteforce TSP cez všetky poradia miest.

---

## 7. Ako počítať zložitosť jednoduchých programov

Pravidlá:

- postupnosť konštantných príkazov je `O(1)`,
- jedna slučka cez `n` prvkov je `O(n)`,
- dve vnorené slučky cez `n` sú `O(n^2)`,
- ak sa premenná v cykle delí dvoma, je to `O(log n)`,
- ak sa vykonajú dve časti za sebou, sčítajú sa: `O(f(n) + g(n))`,
- pri asymptotike dominuje najväčší člen.

Príklad:

```c
for (int i = 0; i < n; i++) {
    // O(1)
}
```

Zložitosť: `O(n)`.

```c
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
        // O(1)
    }
}
```

Zložitosť: `O(n^2)`.

```c
while (n > 1) {
    n = n / 2;
}
```

Zložitosť: `O(log n)`.

---

## 8. Rekurzia a zložitosť

Pri rekurzii sa zložitosť často zapisuje rekurentne.

Príklad merge sort:

```text
T(n) = 2T(n/2) + O(n)
```

Výsledok:

```text
T(n) = O(n log n)
```

Vysvetlenie:

- problém sa rozdelí na dve polovice,
- na každej úrovni sa spracuje dokopy `O(n)` práce,
- úrovní je `log n`.

Príklad binárne vyhľadávanie:

```text
T(n) = T(n/2) + O(1) = O(log n)
```

---

## 9. Amortizovaná zložitosť

Amortizovaná zložitosť hodnotí cenu operácie v celej postupnosti operácií.

Pointa:

- jedna konkrétna operácia môže byť drahá,
- ale nemôže sa diať príliš často,
- preto priemer cez najhoršiu možnú sekvenciu operácií môže byť nízky.

Dôležité:

- amortizovaná zložitosť nie je rovnaká ako priemerný prípad,
- priemerný prípad potrebuje pravdepodobnostný model vstupov,
- amortizovaná zložitosť dáva garanciu pre ľubovoľnú sekvenciu operácií.

---

## 10. Príklad: dynamické pole

Dynamické pole má kapacitu a aktuálnu veľkosť.

Operácia `append`:

- ak je voľné miesto, vloženie trvá `O(1)`,
- ak je pole plné, alokuje sa väčšie pole a staré prvky sa skopírujú,
- resize trvá `O(n)`.

Ak sa kapacita pri resize zdvojnásobuje, operácia `append` má amortizovane `O(1)`.

Prečo:

- drahé kopírovanie nastane len pri veľkostiach približne `1, 2, 4, 8, ...`,
- pri `n` vloženiach sa celkovo kopíruje najviac približne `1 + 2 + 4 + ... + n`,
- tento súčet je `O(n)`,
- teda `n` operácií stojí dokopy `O(n)`,
- jedna operácia stojí amortizovane `O(1)`.

Ak by sme kapacitu zväčšovali vždy len o konštantu, napríklad o 10 prvkov, amortizovaná cena by bola horšia, typicky `O(n)` na jedno vloženie.

---

## 11. Metódy amortizovanej analýzy

Netreba rozoberať dlho, ale dobré poznať názvy.

### Agregovaná metóda

- spočítam cenu celej sekvencie operácií,
- vydelím počtom operácií.

Príklad:

- `n` appendov do dynamického poľa stojí spolu `O(n)`,
- preto jeden append amortizovane `O(1)`.

### Účtovná metóda

- lacným operáciám priradím trochu vyššiu cenu,
- prebytok si „odložím“ na drahú operáciu.

Príklad:

- pri každom appende si akoby zaplatím malú rezervu,
- resize sa potom zaplatí z rezervy uložených operácií.

### Potenciálová metóda

- dátová štruktúra má potenciál, teda uloženú „energiu“ podľa svojho stavu,
- drahá operácia môže znížiť potenciál,
- amortizovaná cena = skutočná cena + zmena potenciálu.

---

## 12. Ďalšie príklady amortizácie

### Zásobník s operáciou multipop

Operácie:

- `push`: vloží prvok,
- `pop`: vyberie jeden prvok,
- `multipop(k)`: vyberie až `k` prvkov.

Jedno `multipop` môže stáť `O(n)`, ale každý prvok môže byť vybratý len raz po tom, čo bol vložený. Preto sekvencia operácií má dobrú amortizovanú cenu.

### Binárny čítač

Pri inkrementácii binárneho čítača sa niekedy preklopí veľa bitov, ale väčšina bitov sa nepreklápa často. Celkový počet preklopení cez veľa inkrementácií dáva amortizovane konštantnú cenu na bitové operácie nižších rádov.

---

## 13. Väzba na bakalárku

V bakalárke sa zložitosť objavuje na viacerých miestach.

### Subset-sum

Problém súčtu podmnožiny má vstup:

- `n` váh,
- cieľový súčet.

Brute force skúša všetky podmnožiny.

- počet podmnožín je `2^n`,
- čas je teda exponenciálny,
- prakticky to rýchlo prestane byť použiteľné.

To priamo súvisí s tým, prečo bol subset-sum zaujímavý pre kryptografiu.

### Meet-in-the-middle

Meet-in-the-middle rozdelí váhy na dve polovice.

- pre každú polovicu vytvorí všetky možné súčty,
- potom hľadá dvojice súčtov, ktoré dajú cieľ.

Zložitosť:

- čas približne `O(2^(n/2))`, plus triedenie alebo vyhľadávanie,
- pamäť približne `O(2^(n/2))`.

Je to výrazne lepšie ako brute force `O(2^n)`, ale stále exponenciálne a pamäťovo drahé.

### Merkle--Hellman šifrovanie

Šifrovanie v Merkle--Hellmanovej schéme počíta:

```text
c = sum x_i b_i
```

Prechádza sa `n` verejných váh.

Na vysokej úrovni:

- čas `O(n)`,
- lebo pre každý bit správy sa rozhodne, či sa váha pripočíta.

Presnejšie:

- ak rátame bitovú dĺžku veľkých čísel, sčítanie veľkých čísel nie je zadarmo,
- potom závisí aj od bitovej veľkosti váh.

### Merkle--Hellman dešifrovanie

Dešifrovanie:

1. vynásobí šifrový text modulárnou inverziou,
2. prevedie ho späť na súčet nad superrastúcou postupnosťou,
3. použije greedy algoritmus od najväčšej váhy.

Na vysokej úrovni greedy čas `O(n)`.

### Generovanie kľúčov

Generovanie kľúčov v implementácii bolo drahšie, lebo:

- generujú sa veľké čísla,
- vytvára sa superrastúca postupnosť,
- počíta sa modul,
- aplikuje sa modulárne násobenie na každú váhu,
- pri iterovanom variante sa transformácie opakujú vo viacerých vrstvách.

Preto v experimentoch pri veľkých `n` často dominoval práve key generation.

---

## 14. Krátka ústna odpoveď

Výpočtová zložitosť opisuje, koľko času alebo pamäte potrebuje algoritmus vzhľadom na veľkosť vstupu. Najčastejšie sledujeme časovú zložitosť ako počet elementárnych operácií. Pri asymptotickej zložitosti nás nezaujíma presný čas na konkrétnom počítači, ale rast funkcie pre veľké vstupy. Preto ignorujeme konštanty a nižšie členy.

Notácia veľké O je horný asymptotický odhad. Formálne `f(n) ∈ O(g(n))`, ak existujú konštanty `c > 0` a `n0 > 0` také, že pre všetky `n >= n0` platí `0 <= f(n) <= c*g(n)`. Napríklad `3n + 7` je `O(n)` a `2n^2 + 100n` je `O(n^2)`. Typické triedy sú `O(1)`, `O(log n)`, `O(n)`, `O(n log n)`, `O(n^2)` a exponenciálne `O(2^n)`.

Amortizovaná zložitosť hodnotí cenu operácie cez celú sekvenciu operácií. Jedna operácia môže byť drahá, ale ak sa nemôže opakovať príliš často, priemerná cena v najhoršej sekvencii môže byť nízka. Klasický príklad je dynamické pole. Bežný `append` je `O(1)`, občasný resize je `O(n)`, ale ak sa kapacita zdvojnásobuje, `n` appendov stojí spolu `O(n)`, takže jeden append je amortizovane `O(1)`.

V mojej bakalárke sa to objavuje napríklad pri subset-sum. Brute force skúša všetky podmnožiny, teda má exponenciálnu zložitosť `O(2^n)`. Meet-in-the-middle znižuje čas približne na `O(2^(n/2))`, ale potrebuje veľa pamäte. Naopak samotné šifrovanie v Merkle--Hellmanovej schéme je na vysokej úrovni lineárne v počte váh, lebo iba sčíta vybrané verejné váhy podľa bitov správy.

---

## 15. Možné doplňujúce otázky

### Prečo ignorujeme konštanty?

Lebo asymptotika porovnáva rast pre veľké vstupy. Konštantný faktor môže byť dôležitý v praxi, ale nemení triedu rastu.

### Je `O(n)` aj `O(n^2)`?

Formálne áno, lebo `n` je zhora ohraničené `n^2` pre dostatočne veľké `n`. Ale presnejší/tiesnejší odhad je `O(n)` alebo `Θ(n)`.

### Rozdiel medzi worst-case a amortized?

Worst-case hodnotí jednu najhoršiu operáciu alebo beh. Amortized hodnotí priemernú cenu operácie v najhoršej možnej sekvencii operácií.

### Rozdiel medzi average-case a amortized?

Average-case potrebuje pravdepodobnostné rozdelenie vstupov. Amortized nepotrebuje pravdepodobnosť; dáva garanciu cez každú sekvenciu operácií.

### Prečo dynamické pole append nie je worst-case `O(1)`?

Lebo pri resize musí skopírovať existujúce prvky do väčšieho poľa, čo stojí `O(n)`. Amortizovane je však `O(1)`, ak sa kapacita násobí.

### Aká je zložitosť brute force subset-sum?

Skúša všetky podmnožiny, teda `2^n` možností. Ak pri každej možnosti počítam súčet naivne, môže tam byť aj faktor `n`, ale hlavný rast je exponenciálny.

### Prečo meet-in-the-middle pomáha?

Namiesto všetkých `2^n` podmnožín naraz vytvorí súčty pre dve polovice veľkosti `n/2`. Tých je `2^(n/2)` pre každú polovicu. Čas a pamäť sú stále exponenciálne, ale exponent je polovičný.
