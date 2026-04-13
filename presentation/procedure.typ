#import "@preview/touying:0.7.1": *
#import themes.metropolis: *


#show: metropolis-theme.with(
  aspect-ratio: "16-9",
  footer: self => self.info.institution,
  config-info(
    title: [Postup práce],
    subtitle: [Knapsack cryptosystems and its variations],
    author: [David Vavrinek],
    //date: datetime.today(),
    // institution: [Institution],
    // contact: [varinek1\@uniba.sk],
    // logo: emoji.city,
  ),
)

#set heading()

#title-slide()

== Aktuálny stav
- návrhovú fázu práce mám uzavretú
- teoretický základ a štruktúru práce mám rozpracované
- teraz sa presúvam od opisu schém k ich analýze a porovnaniu

== Ako mám prácu rozdelenú
- teoretická časť pokrýva klasický Merkle-Hellman, jeho slabiny a vybrané varianty
- implementačná časť sa sústreďuje na keygen, encrypt a decrypt
- záverečná časť bude porovnávať bezpečnosť, zložitosť a praktickú realizáciu

== Čo už funguje
- klasický Merkle-Hellman mám implementovaný
- permutovaný variant mám implementovaný
- seedované spúšťanie a základ pre experimenty fungujú

== Čo ešte chýba
- chýba mi dokončiť názornú demonštráciu útoku na klasickú schému
- potrebujem dopracovať porovnanie variantov a ich slabín
- ešte musím dopísať implementačnú, výsledkovú a záverečnú časť práce

== Ďalší postup
- dokončím attack/demo časť pre klasický Merkle-Hellman
- doplním porovnávacie experimenty a priebežné výsledky
- zapracujem tieto výstupy do textu práce a na web
