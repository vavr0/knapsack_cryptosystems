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

== Stav práce

- implementovaný `mh-classic`
- implementovaný `mh-permuted` ako ďalší experimentálny variant
- hotové spoločné rozhranie schém a CLI režimy `demo` a `bench`
- ďalší krok: attack/demo časť a systematické experimenty

== Rozšíriteľnosť riešenia

- všetky varianty schémy sú skryté za spoločným API
- návrh je zameraný na jednoduché pridávanie nových variantov
- aplikácia potom volá len `keygen`, `encrypt`, `decrypt`

```c
typedef struct {
  KnapStatus (*keygen)(...);
  KnapStatus (*encrypt)(...);
  KnapStatus (*decrypt)(...);
} SchemeOps;

const SchemeOps *scheme_resolve(const char *id) {
  if (!id || strcmp(id, "mh-classic") == 0) return scheme_mh_get();
  if (strcmp(id, "mh-permuted") == 0) return scheme_mh_permuted_get();
  // planned next step
  if (strcmp(id, "mh-iterated") == 0) return scheme_mh_iterated_get();
  return NULL;
}
```

== MH classic: tvorba kľúča

- privátne váhy generované ako superincreasing sekvencia
- používam *GMP* (`mpz_t`) na veľké celé čísla
- verejný kľúč vzniká ako `b_i = (w_i * r) mod m`

```c
u64 margin_u64 = 1 + (prng_rand_u64(rng) % (64u * key->n));
mpz_set_ui(margin, margin_u64);
mpz_add(key->mod, sum, margin);

for (;;) {
  mpz_set_ui(key->mult, prng_rand_u64(rng));
  mpz_mod(key->mult, key->mult, key->mod);
  if (mpz_invert(key->mult_inv, key->mult, key->mod) != 0) break;
}

for (u64 i = 0; i < key->n; i++) {
  mpz_mul(key->pub_weights[i], key->priv_weights[i], key->mult);
  mpz_mod(key->pub_weights[i], key->pub_weights[i], key->mod);
}
```

== Beh schémy a chybové stavy

- flow je rovnaký pre všetky varianty implementácie
- `KnapStatus` rozlišuje `INVALID`, `ALLOC`, `CRYPTO`, `INTERNAL`
- jednotné spracovanie chýb naprieč implementáciou

```c
typedef enum {
  KNAP_OK, KNAP_ERR_INVALID, KNAP_ERR_ALLOC,
  KNAP_ERR_CRYPTO, KNAP_ERR_INTERNAL
} KnapStatus;

status = scheme->keygen(&params, &scheme_key);
if (status != KNAP_OK) return status;

status = scheme->encrypt(&scheme_key, bit_buf_view(&flags->message_bits),
                         ciphertext);
if (status != KNAP_OK) return status;

status = scheme->decrypt(&scheme_key, ciphertext, &decrypted);
if (status != KNAP_OK) return status;
```

== Demo a benchmark režim

- `demo`: end-to-end keygen / encrypt / decrypt na konkrétnej bitovej správe
- `bench`: opakované meranie `keygen`, `encrypt`, `decrypt`
- CSV výstup slúži ako základ pre porovnanie variantov a parametrov

```bash
./code/build/knapsack demo --scheme mh-classic --msg 101010 --seed 123
./code/build/knapsack bench --scheme mh-classic --n 128 --reps 10 \
  --seed 123 --format csv
```

== Smerovanie práce

- preštudoval som literatúru k variantom a útokom na knapsack kryptosystémy
- moderný výskum som si prešiel skôr pre kontext, nie ako hlavnú časť práce
- väčšina novších výsledkov sa zameriava skôr na kryptanalýzu než návrh nových schém
- knapsack prístupy sa dnes pri návrhu reálnych kryptosystémov nepoužívajú
- jadrom práce zostáva implementácia variantov a ich porovnanie

== Čo ešte treba urobiť

- dokončiť attack/demo časť pre klasickú schému
- doplniť spracovanie správ (bloky, plaintext input)
- pripraviť `mh-iterated`
- spraviť porovnávacie experimenty a dopísať *Implementation and Methodology* a *Results*
