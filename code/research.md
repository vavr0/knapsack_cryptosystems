# Moderný vývoj knapsack / subset-sum kryptografie

## Kontext
- Klasické knapsack kryptosystémy (Merkle–Hellman) sú zlomené
- Subset sum ako problém stále relevantný
- Dnes:
  - teoretický hard problém
  - kryptanalýza
  - nie praktické PKC

## 1. Teoretická vetva (provable security)

Impagliazzo–Naor → Lyubashevsky–Palacio–Segev → Faust–Masny–Venturi

- Subset sum ako hardnostný predpoklad
- Bezpečnosť cez redukciu (break ⇒ solve subset sum)

Vývoj:
- Impagliazzo–Naor: PRG, one-way funkcie  
  https://pdfs.semanticscholar.org/0339/ce2081cce967786c46faa5e1ee4e0fa15516.pdf

- Lyubashevsky–Palacio–Segev: public-key primitíva  
  https://www.iacr.org/archive/tcc2010/59780380/59780380.pdf

- Faust–Masny–Venturi: CCA-secure PKE  
  https://eprint.iacr.org/2015/1223.pdf

Poznámka:
- nie prelomené
- nie prakticky používané

## 2. Moderné algoritmy / kryptanalýza

- smer vývoja:
  - lepšie riešenie subset sum
  - nie nové šifry

Príklady:
- Joux–Jobic / Joux–Węgrzycki (2020s)  
  https://arxiv.org/pdf/2408.16108

Základ:
- meet-in-the-middle
- reprezentácie
- lattice (LLL)
- hustota

Význam:
- ovplyvňuje bezpečnosť schém
- nastavovanie parametrov

## 3. Nové knapsack-like návrhy

### PV-Knap / Vandermonde meets Regev (2022)

- PKE návrh
- algebraická štruktúra
- inšpirácia LWE  
  https://eprint.iacr.org/2022/679.pdf

Kryptanalýza:
- key recovery attack (2024)  
  https://eprint.iacr.org/2024/366.pdf

Záver:
- výskumne zaujímavé
- zatiaľ krehké

## 4. Post-quantum kryptografia

NIST štandardy:
- ML-KEM (Kyber)
- ML-DSA (Dilithium)
- SLH-DSA (SPHINCS+)

Základ:
- lattice
- code-based

Poznámka:
- nie subset sum

## 5. Subset sum a kvantá

- OTU (historicky):
  - subset-sum-based PKC
  - dnes skôr experimentálne

- kvantové algoritmy pre subset sum:  
  https://eprint.iacr.org/2013/199.pdf

- ZK / subset-sum súvislosti:  
  https://eprint.iacr.org/2022/223.pdf

Záver:
- subset sum nie je zlomený kvantami
- ale ani sa nepoužíva v PQ praxi

## 6. Reálny význam dnes

Subset sum:

-  nie:
  - praktické PKC

-  áno:
  - kryptanalýza
  - teória
  - výskum algoritmov

## Zhrnutie

Knapsack kryptosystémy sú historicky prekonané, ale subset sum prežil ako
teoretický problém a nástroj kryptanalýzy; moderná post-quantum kryptografia je
založená najmä na lattice a code-based prístupoch.

## LINKS

- https://www.iacr.org/archive/tcc2010/59780380/59780380.pdf
- https://eprint.iacr.org/2015/1223.pdf?utm_source=chatgpt.com
- https://arxiv.org/pdf/2408.16108 https://eprint.iacr.org/2022/679.pdf
- https://eprint.iacr.org/2024/366.pdf
- https://eprint.iacr.org/2022/223.pdf?utm_source=chatgpt.com
- https://eprint.iacr.org/2013/199.pdf
- https://pdfs.semanticscholar.org/0339/ce2081cce967786c46faa5e1ee4e0fa15516.pdf
