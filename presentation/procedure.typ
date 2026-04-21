#import "@preview/touying:0.7.1": *
#import themes.metropolis: *

#show: metropolis-theme.with(
  aspect-ratio: "16-9",
  footer: self => self.info.institution,
  config-info(
    title: [Postup práce],
    subtitle: [Classical Knapsack Cryptosystem and its Variations],
    author: [David Vavřinek],
  ),
)

#show raw: set text(font: "SauceCodePro NF")
#show raw.where(block: true): set text(size: 0.85em)
#set heading(numbering: none)

#title-slide()

== Stav implementácie

- `mh-classic` je hotový a slúži ako baseline
- `mh-permuted` je implementovaný ako experimentálny variant
- hotové režimy `demo` a `bench`
- náhodné generovanie mám riadené cez seed, takže experimenty viem spoľahlivo zopakovať

== Architektúra implementácie

- `main.c`: entrypoint
- `app.c`: demo flow
- `bench.c`: benchmarky
- `cli.c`: spracovanie argumentov
- `scheme_*.c`: samostatné implementácie variantov
- `demo` a `bench` prepínajú varianty cez malé spoločné rozhranie
```c
typedef struct {
  KnapStatus (*keygen)(...);
  KnapStatus (*encrypt)(...);
  KnapStatus (*decrypt)(...);
} SchemeOps;
```
== MH classic a GMP

- implementácia používa `GMP` (`mpz_t`) na prácu s veľkými číslami
- na ukážke je časť generovania kľúča pre `mh-classic`
- `GMP` používam pri práci so superincreasing sekvenciou, modulom a multiplikátorom

```c
for (u64 i = 0; i < key->n; i++) {
  mpz_set_ui(delta, 1 + (prng_rand(rng) % 16u));
  mpz_add(delta, delta, sum);
  mpz_set(key->priv_weights[i], delta);
  mpz_add(sum, sum, key->priv_weights[i]);
}
u64 margin_u64 = 1 + (prng_rand_u64(rng) % (64u * key->n));
mpz_set_ui(margin, margin_u64);
mpz_add(key->mod, sum, margin);

for (;;) {
  mpz_set_ui(key->mult, prng_rand_u64(rng));
  mpz_mod(key->mult, key->mult, key->mod);
  if (mpz_invert(key->mult_inv, key->mult, key->mod) != 0) break;
}
```

== Demo a benchmark režim

- `demo`: end-to-end `keygen / encrypt / decrypt` na konkrétnej bitovej správe
- `bench`: opakované meranie `keygen`, `encrypt`, `decrypt`
- benchmark vie exportovať výsledky do CSV pre ďalšie spracovanie
- plaintext spracovanie mám rozrobené ako rozšírenie `demo` režimu
```bash
./code/build/knapsack demo --scheme mh-classic --bits 101010 --seed 123
./code/build/knapsack bench --scheme mh-classic --n 128 --reps 10 \
  --seed 123 --format csv
```
== Rozpracované časti a ďalší postup

- dotiahnuť plaintext vstup v `demo` vrátane blokového spracovania
- zrefaktorovať `mh-permuted`
- doimplementovať `mh-iterated` ako ďalší variant
- pripraviť systematické benchmarky
- doplniť prvé útoky na `mh-classic`
