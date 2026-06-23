#import "@preview/touying:0.7.1": *
#import themes.metropolis: *

#show: metropolis-theme.with(
  aspect-ratio: "16-9",
  footer: self => self.info.institution,
  config-info(
    title: [Klasický batohový kryptosystém a jeho varianty],
    subtitle: [Classical Knapsack Cryptosystem and its Variations],
    author: [
      #grid(
        columns: (auto, 1fr),
        gutter: 0.7em,
        row-gutter: 0.50em,
        [Autor:], [David Vavřinek],
        [Vedúca práce:], [doc. RNDr. Tatiana Jajcayová, PhD.],
      )
    ],
    institution: [FMFI UK],
  ),
)

#show raw.where(block: true): set text(size: 0.74em)
#set heading(numbering: none)
#set par(leading: 0.22em)
#set list(spacing: 0.90em)

#let codebox(body) = block(
  fill: rgb("#f7f7f7"),
  stroke: rgb("#d0d0d0"),
  radius: 4pt,
  inset: (x: 10pt, y: 8pt),
  width: 100%,
)[
  #set text(size: 1.00em)
  #set par(leading: 0.28em)
  #body
]

#let graphslide(path, notes) = grid(
  columns: (1.3fr, 0.9fr),
  gutter: 0.3em,
  align(left + horizon)[#image(path, width: 100%)],
  align(horizon)[
    #set text(size: 1.00em)
    #notes
  ],
)

#title-slide()

== Motivácia a kontext

- problém zdieľania tajného kľúča pri symetrickej kryptografii
- verejný a súkromný kľúč, asymetria výpočtu
- subset-sum ako zaujímavý základ bez známeho efektívneho algoritmu
- práca sleduje Merkle--Hellman, zlyhanie, varianty a implementáciu

== Subset-sum a ľahký prípad

#align(center)[$ sum_(i=1)^n x_i a_i = S, quad x_i in {0, 1} $]

- všeobecne hľadáme podmnožinu váh s daným súčtom
- rozhodovacia verzia sa pýta, či taká podmnožina existuje
- táto rozhodovacia verzia je NP-úplná
- superrastúca postupnosť je špeciálny riešiteľný prípad
- greedy postupuje od najväčšej váhy nadol
- menšie váhy nedokážu nahradiť jednu väčšiu

== Merkle--Hellmanova schéma

- generovanie kľúčov vytvorí superrastúcu postupnosť $w$ a čísla $m$, $r$
- platí $m > sum w_i$ a $gcd(r, m) = 1$, preto existuje inverzia modulo $m$
#table(
  columns: (1.05fr, 3.0fr),
  stroke: none,
  inset: (x: 4pt, y: 5pt),
  [verejný kľúč], [$ b_i equiv r w_i mod m $],
  [šifrovanie], [$ C = sum_(i=1)^n x_i b_i $],
  [dešifrovanie], [$ C' equiv r^(-1) C mod m $],
)

- po dešifrovacej transformácii sa rieši pôvodná superrastúca postupnosť
- trapdoor je návrat zo zdanlivo ťažkej verejnej inštancie na ľahko riešiteľný prípad

== Prečo konštrukcia zlyhala

- verejný kľúč nevzniká ako náhodná všeobecná inštancia
- vzniká prevodom zo súkromnej superrastúcej štruktúry
- Shamirov útok využíva vzťahy medzi verejnými váhami
- hľadá inú transformáciu späť na ľahký prípad
- nízka hustota umožňuje mriežkové útoky, napríklad LLL
- NP-úplnosť všeobecného problému sama osebe nestačí

== Varianty a útoky

#table(
  columns: (1.15fr, 2.25fr, 2.10fr),
  inset: (x: 8pt, y: 10pt),
  [*Schéma*], [*Myšlienka*], [*Slabina*],
  [Merkle--Hellman], [skrytá superrastúca postupnosť], [Shamirov útok],
  [permuted MH], [tajná permutácia poradia váh], [vzťah k súkromným váham zostáva],
  [iterated MH], [viac modulárnych vrstiev], [Brickell, mriežkové metódy],
  [Chor--Rivest], [trapdoor v konečných poliach], [algebraická štruktúra verejného kľúča],
)

== Implementácia

- jazyk C
- veľké celé čísla pomocou GMP (`mpz_t`)
- varianty `mh-classic`, `mh-permuted`, `mh-iterated`
- spoločné rozhranie `keygen`, `encrypt`, `decrypt`, cleanup

== Ukážka CLI

- režimy `demo` a `bench`
- voľby schémy, vstupu, veľkosti bloku a seedu
- text sa prevedie na bity a rozdelí do blokov
- bez seedu OS entropia, so seedom reprodukovateľný beh
- `bench` meria keygen, encrypt, decrypt do CSV

#codebox[
```text
$ ./knapsack demo --scheme mh --msg "two block message" --n 128 --seed 123
Scheme: mh-classic
Block-Size: 128
Blocks: 2
Ciphertext:
146139970358749417963450435298092519600081
9592959278104647334923768299118570459183
Decrypted-Text: two block message
Status: OK
```
]

== Experimentálne nastavenie

- release build C programu
- Python skripty pre spustenie, CSV a grafy
- veľkosti blokov $n in {128, 256, 512, 1024, 2048, 4096}$
- 3 seedy; warm-up; priemer z 5 meraní
- kontrola správneho dešifrovania

== Výsledok: runtime variantov

#graphslide("../code/experiments/plots/crypto_total_with_iterated_layers.png")[
- pri $n=4096$:
  - classic ≈ 11 ms
  - permuted ≈ 12 ms
  - iterated ≈ 31 ms
- viac vrstiev zvyšuje cenu keygenu
]

== Výsledok: čo stojí najviac času

#graphslide("../code/experiments/plots/crypto_components_classic.png")[
- v klasickom variante dominuje generovanie kľúča
- pri $n=4096$ keygen ≈ 10.9 ms
- po vytvorení kľúčov je komunikácia veľmi lacná
- problém bol v štruktúre verejného kľúča, nie v efektivite
]

== Hustota pri iterovaní

#graphslide("../code/experiments/plots/iterated_layers_density.png")[
- hustota $n / log_2(max b_i)$
- viac vrstiev zväčšuje verejné váhy
- pri $n=128$ dve vrstvy ≈ 0.916, päť vrstiev ≈ 0.812
]

== Vplyv parametra delta

#graphslide("../code/experiments/plots/sweep_delta_density.png")[
- `delta_max` je horná hranica prírastku súkromnej váhy
- väčšie `delta_max` zväčšuje bitovú veľkosť verejných váh
- pri $n=128$ hustota klesla z 1.000 na približne 0.948
- `margin_factor` mal v tejto implementácii menší vplyv
]

== Hlavné prínosy práce

- prehľad a systematizácia klasických batohových kryptosystémov
- vysvetlenie zlyhania cez rozdiel medzi ťažkým problémom a štruktúrou kľúčov
- C/GMP implementácia troch Merkle--Hellmanových variantov
- demo a benchmark režim
- reprodukovateľné merania a grafy

== Záver

- klasické batohové kryptosystémy dnes nie sú vhodné na praktické šifrovanie
- ich význam je najmä historický a didaktický
- problém súčtu podmnožiny sa v teórii stále študuje
- praktická verejná kryptografia sa vydala inými smermi, napríklad RSA a eliptické krivky
- RSA ako kontrast, faktorizácia namiesto NP-úplného problému

#align(center)[Ďakujem za pozornosť.]

== Otázky k práci

- otázky z posudku oponenta: doplniť po doručení

== Otázky k rozprave

- 11 P: Asymptotická výpočtová zložitosť, veľké O, amortizovaná zložitosť
- 2 A: Bezpečnosť sietí; bezpečnostné mechanizmy na vrstvách; VLAN
