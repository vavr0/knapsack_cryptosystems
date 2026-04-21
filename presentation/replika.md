# Postup práce

Dnes by som chcel stručne ukázať aktuálny stav implementačnej časti mojej bakalárskej práce. V tejto prezentácii sa už nesústredím na cieľ práce, ten som rozoberal skôr, ale hlavne na to, čo je už implementované, čo je momentálne rozpracované a aké budú najbližšie technické kroky.

# Stav implementácie

Momentálne mám hotový variant `mh-classic`, ktorý používam ako základný baseline pre ďalšie porovnanie. Okrem toho mám implementovaný aj `mh-permuted` ako experimentálny variant. Z pohľadu používania programu už mám pripravené dva hlavné režimy, a to `demo` a `bench`.

Režim `demo` používam na end-to-end overenie, teda na vygenerovanie kľúča, zašifrovanie a následné dešifrovanie konkrétnej správy. Režim `bench` používam na opakované merania tých istých operácií. Dôležité je aj to, že náhodné generovanie mám riadené cez seed, takže experimenty viem spoľahlivo zopakovať a porovnávať pri rovnakých podmienkach.

# Architektúra implementácie

Pri implementácii som sa snažil držať čo najjednoduchšiu architektúru bez zbytočne veľkého frameworku. `main.c` slúži len ako entrypoint programu. `app.c` rieši demo flow, `bench.c` benchmarkovú časť a `cli.c` spracovanie argumentov príkazového riadku. Samotné kryptografické varianty sú potom oddelené v samostatných `scheme_*.c` súboroch.

Zároveň používam malé spoločné rozhranie pre operácie `keygen`, `encrypt` a `decrypt`, aby som vedel v `demo` a `bench` režime jednoducho prepínať medzi variantmi bez toho, aby som ich implementácie zlieval dokopy. Cieľom tu bolo mať riešenie, ktoré je jednoduché, prehľadné a vhodné na ďalšie rozšírenie o ďalšie varianty.

# MH classic a GMP

Na tomto slajde je ukážka časti generovania kľúča pre `mh-classic`. Hlavný dôvod, prečo ju tu ukazujem, je použitie knižnice `GMP`, konkrétne typu `mpz_t`, pretože pri implementácii pracujem s veľkými celými číslami a nechcel som byť obmedzený len štandardnými celočíselnými typmi jazyka C.

Pri generovaní kľúča vytváram superincreasing sekvenciu, potom z nej určujem modul väčší než jej súčet a následne vyberám multiplikátor, ku ktorému existuje modulárny inverz. Nejde mi teraz o vysvetlenie každého riadku kódu, ale skôr o to ukázať, že implementácia už nie je len návrh, ale reálne fungujúci kód, na ktorom viem ďalej stavať ďalšie varianty aj experimenty.

# Demo a benchmark režim

Z pohľadu používania programu mám dnes dve hlavné cesty. V režime `demo` viem na konkrétnej bitovej správe spraviť celý tok `keygen`, `encrypt` a `decrypt`, takže si viem rýchlo overiť korektnosť implementácie. V režime `bench` viem tie isté operácie spúšťať opakovane a získavať z nich merania, ktoré viem exportovať aj do CSV formátu pre ďalšie spracovanie.

Toto je dôležité hlavne pre neskoršie systematické porovnanie variantov a parametrov. Plaintext vstup mám momentálne rozpracovaný ako rozšírenie `demo` režimu, aby som nebol obmedzený len na priamy bitový vstup.

# Rozpracované časti a ďalší postup

Najbližšie technické kroky sú momentálne tieto. V `demo` režime chcem dotiahnuť plaintext vstup vrátane blokového spracovania správy. Ďalej chcem zrefaktorovať `mh-permuted`, aby bola táto varianta čistejšia a jednoduchšie sa s ňou ďalej pracovalo. Následne chcem doimplementovať `mh-iterated` ako ďalší variant vhodný na porovnanie s baseline.

Keď budú tieto varianty v stabilnejšom stave, pripravím systematické benchmarky a potom doplním aj prvé útoky na `mh-classic`. Ďalšia fáza práce teda nebude len o samotnej implementácii, ale aj o rozumnom porovnaní jednotlivých variantov a ukázaní ich praktických vlastností a slabín.

# Záver

To je zatiaľ z mojej strany všetko. Momentálne mám hotový funkčný baseline a infraštruktúru na demo aj merania, a najbližšie sa budem sústrediť hlavne na dopracovanie variantov, benchmarkov a následne útokovej časti.
