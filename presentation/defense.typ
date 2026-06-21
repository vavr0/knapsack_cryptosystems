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

#show raw.where(block: true): set text(size: 0.78em)
#set heading(numbering: none)
#set par(leading: 0.22em)
#set list(spacing: 0.75em)

#let emphbox(body) = block(
  fill: rgb("#fff7ef"),
  stroke: (left: 4pt + rgb("#eb811b")),
  radius: 4pt,
  inset: (x: 10pt, y: 7pt),
  width: 100%,
  body,
)

#let terminal(body) = block(
  fill: rgb("#1f1f1f"),
  radius: 6pt,
  inset: (x: 10pt, y: 8pt),
  width: 100%,
)[
  #set text(fill: rgb("#f5f5f5"), size: 0.70em)
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

#emphbox[
  Ako môže kryptosystém založený na ťažkom probléme zlyhať?
]

- verejnokľúčová kryptografia rieši problém zdieľania tajného kľúča
- batohové kryptosystémy: historický pokus použiť problém súčtu podmnožiny
- Merkle--Hellman: názorný príklad trapdooru
- práca: prehľad konštrukcií a útokov + implementácia a experimenty

== Subset-sum a ľahký prípad

#align(center)[$ sum_(i=1)^n x_i a_i = S, quad x_i in {0, 1} $]

- hľadáme podmnožinu váh s daným súčtom
- rozhodovacia verzia je NP-úplná
- v kryptosystéme hľadáme konkrétne bity správy
- superrastúca postupnosť je ľahký prípad riešiteľný greedy algoritmom

#emphbox[
  V Merkle--Hellmanovi je ľahký prípad skrytý v súkromnom kľúči.
]

== Merkle--Hellmanova schéma

#align(center)[
  súkromná ľahká inštancia $arrow.r$ verejná zdanlivo ťažká inštancia
]

- súkromný kľúč: superrastúca postupnosť $w$
- zvolí sa $m > sum w_i$ a $r$ tak, že $gcd(r, m) = 1$

#align(center)[
  $ b_i equiv r w_i mod m $

  $ C = sum_(i=1)^n x_i b_i $

  $ C' equiv r^(-1) C mod m $
]

- po inverzii sa použije greedy algoritmus

== Prečo konštrukcia zlyhala

- verejný kľúč nevzniká ako náhodná všeobecná inštancia
- vzniká prevodom zo súkromnej superrastúcej štruktúry
- Shamir: ekvivalentný trapdoor v polynomiálnom čase
- nízka hustota: mriežkové útoky a LLL

#emphbox[
  NP-úplnosť všeobecného problému sama osebe nezaručuje bezpečnosť konkrétnej schémy.
]

== Varianty a útoky

#table(
  columns: (1.2fr, 2.0fr, 2.0fr),
  inset: 6pt,
  [*Schéma*], [*Myšlienka*], [*Slabina*],
  [Merkle--Hellman], [modulárne skrytá superrastúca postupnosť], [Shamirov útok],
  [permuted MH], [skrytá permutácia váh], [stále rovnaká trapdoor štruktúra],
  [iterated MH], [viac modulárnych vrstiev], [Brickell, mriežkové metódy],
  [Chor--Rivest], [konečné polia, vyššia hustota], [algebraická štruktúra],
)

== Implementácia

- jazyk: C
- veľké celé čísla: GMP (`mpz_t`)
- varianty: `mh-classic`, `mh-permuted`, `mh-iterated`
- spoločné rozhranie: `keygen`, `encrypt`, `decrypt`, cleanup
- PCG generátor; bežný seed z OS entropie, experimenty s explicitným seedom

== Ukážka CLI

#grid(
  columns: (1.35fr, 0.75fr),
  gutter: 0.6em,
  align(horizon)[
    #terminal[
```text
$ ./knapsack demo --scheme mh-classic --msg "hello" --n 40 --seed 123
Scheme: mh-classic
Block-Size: 40
Blocks: 1
Ciphertext: 154488066942563
Decrypted-Text: hello
Status: OK
```
    ]
  ],
  align(horizon)[
    - `demo`: celý priebeh
    - `bench`: časy a parametre
    - `attack`: brute force a MITM
    - nie Shamir ani LLL
  ],
)

== Experimentálne nastavenie

- veľkosti blokov: $n in {128, 256, 512, 1024, 2048, 4096}$
- tri deterministické seedy; warm-up a merané opakovania
- benchmark overí aj korektné dešifrovanie
- Python skripty iba spúšťajú C program a generujú grafy
- sledované: runtime, hustota, `sum_bits`, margin, MITM tabuľka

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
- pri $n=4096$: keygen ≈ 10.9 ms
- šifrovanie aj dešifrovanie boli pod 0.1 ms
]

== Hustota pri iterovaní

#graphslide("../code/experiments/plots/iterated_layers_density.png")[
- hustota: $n / log_2(max b_i)$
- viac vrstiev zväčšuje verejné váhy
- pri $n=128$: 2 vrstvy ≈ 0.916, 5 vrstiev ≈ 0.812
]

== Vplyv delta a marginu

#graphslide("../code/experiments/plots/sweep_delta_density.png")[
- `delta`: prírastok pri tvorbe súkromnej superrastúcej váhy
- `margin`: rezerva medzi $sum w_i$ a modulom $m$
- väčšie `delta_max` znižuje hustotu
- margin mal v tejto implementácii malý vplyv
]

== Jednoduché solver experimenty

#graphslide("../code/experiments/plots/attack_ms.png")[
- brute force: približne $2^n$ možností
- meet-in-the-middle: približne $2^(n/2)$, ale potrebuje veľa pamäte
- baseline experiment, nie Shamirov ani LLL útok
]

== Hlavné prínosy práce

- prehľad a systematizácia klasických batohových kryptosystémov
- vysvetlenie zlyhania: ťažký problém vs. štruktúra kľúčov
- C/GMP implementácia troch Merkle--Hellmanových variantov
- demo, benchmark a jednoduché solver experimenty
- reprodukovateľné merania a grafy

== Záver

#emphbox[
  Klasické batohové kryptosystémy dnes nie sú vhodné na praktické šifrovanie.
]

- ich význam je najmä historický a didaktický
- problém súčtu podmnožiny sa v teórii stále študuje
- praktická verejná kryptografia sa vydala inými smermi
- napríklad RSA nestojí na NP-úplnom probléme, ale na faktorizácii

#align(center)[*Ďakujem za pozornosť.*]

== Otázky k práci

- otázky z posudku oponenta: doplniť po doručení

== Otázky k rozprave

- 11 P: Asymptotická výpočtová zložitosť, veľké O, amortizovaná zložitosť
- 2 A: Bezpečnosť sietí; bezpečnostné mechanizmy na vrstvách; VLAN
