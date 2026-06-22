# 2A — Bezpečnosť sietí, vrstvy, VLAN

## Ako čítať otázku

Otázka znamená:

> bezpečnostné problémy a mechanizmy na rôznych sieťových vrstvách + detailnejšie VLAN.

VLAN nie je mechanizmus na všetkých vrstvách. VLAN patrí hlavne na linkovú vrstvu.

---

## 1. Ciele sieťovej bezpečnosti

- **Dôvernosť**: údaje vie čítať iba oprávnená strana.
- **Integrita**: údaje neprídu nepozorovane zmenené.
- **Autentickosť**: viem overiť, kto správu alebo spojenie vytvoril.
- **Dostupnosť**: služba je dostupná, keď ju potrebujem.
- **Autentifikácia**: overenie identity používateľa, systému alebo servera.
- **Riadenie prístupu**: kto smie komunikovať, s čím a za akých podmienok.

Základná pointa: sieť je nedôveryhodné prostredie. Útočník môže komunikáciu čítať, meniť, podvrhnúť alebo blokovať.

---

## 2. Typické bezpečnostné problémy

- **Odpočúvanie**: útočník číta prenášané dáta.
- **Modifikácia dát**: útočník zmení obsah správy alebo paketu.
- **Podvrhnutie identity**: útočník sa tvári ako iný používateľ alebo server.
- **Man-in-the-middle**: útočník je medzi dvoma stranami a s každou komunikuje zvlášť.
- **Replay útok**: útočník zopakuje starú platnú komunikáciu.
- **Denial of Service**: cieľom je znefunkčniť službu alebo zahltiť sieť.
- **Nesprávna konfigurácia**: otvorené porty, slabé heslá, zlé VLAN/trunk nastavenia.
- **Vnútorný útočník**: niekto už je v sieti, preto nestačí chrániť len perimiter.

---

## 3. Základné bezpečnostné mechanizmy

- **Šifrovanie**: chráni dôvernosť dát.
- **MAC / HMAC**: kontrola integrity a autentickosti správy pomocou zdieľaného kľúča.
- **Digitálny podpis**: integrita, autentickosť a nepopierateľnosť pôvodu.
- **Certifikáty a PKI**: väzba verejného kľúča na identitu.
- **Autentifikácia**: heslá, certifikáty, 2FA, 802.1X.
- **Autorizácia a ACL**: pravidlá, čo je povolené.
- **Segmentácia siete**: oddelenie častí siete, napríklad VLAN.
- **Firewall**: filtrácia komunikácie podľa pravidiel.
- **VPN**: bezpečný tunel cez nedôveryhodnú sieť.
- **Monitoring a logovanie**: detekcia incidentov.
- **Organizačné opatrenia**: prístupy, politiky, školenia, fyzická ochrana.

---

## 4. Problém distribúcie kľúčov

Kryptografia sama nestačí. Treba vedieť, odkiaľ sa bezpečne vezme správny kľúč.

### Symetrická kryptografia

- Obe strany používajú rovnaký tajný kľúč.
- Problém: ako si ho bezpečne vymenia cez nedôveryhodnú sieť?
- Ak ho útočník získa, vie čítať alebo falšovať komunikáciu.

### Diffie-Hellman

- Umožní dohodnúť spoločný tajný kľúč cez verejný kanál.
- Bez autentifikácie je zraniteľný voči Man-in-the-Middle útoku.
- Preto sa kombinuje s certifikátmi alebo podpismi.

### Asymetrická kryptografia a certifikáty

- Verejný kľúč môže poznať každý, súkromný ostáva tajný.
- Problém: ako viem, že verejný kľúč naozaj patrí danému serveru?
- Riešenie: certifikát podpísaný certifikačnou autoritou.
- Toto používa napríklad TLS/HTTPS.

---

## 5. Bezpečnosť podľa vrstiev

### Fyzická vrstva

Problémy:

- prístup ku káblom, zásuvkám, switchom, serverom,
- poškodenie alebo odpojenie zariadení,
- možnosť pripojiť cudzie zariadenie do siete.

Mechanizmy:

- zamknuté miestnosti a racky,
- kontrola fyzického prístupu,
- oddelené kabeláže pre citlivé siete,
- dokumentácia a monitoring zariadení.

### Linková vrstva

Problémy:

- zariadenie sa pripojí do nesprávnej siete,
- MAC spoofing,
- odpočúvanie v lokálnej sieti,
- útoky na ARP alebo lokálnu komunikáciu,
- zlá konfigurácia switchov a trunk portov.

Mechanizmy:

- **VLAN**: logické oddelenie sietí na linkovej vrstve,
- **802.1X**: autentifikácia zariadenia/používateľa pred prístupom do siete,
- port security,
- WPA2/WPA3 pri WiFi,
- oddelenie administrátorských, serverových a používateľských sietí.

### Sieťová vrstva

Problémy:

- IP spoofing,
- neoprávnené smerovanie,
- skenovanie siete,
- prístup do sietí, kam používateľ nemá patriť,
- útoky medzi subnetmi.

Mechanizmy:

- firewall a ACL,
- stateless/stateful filtrácia,
- routing politiky,
- VPN,
- IPsec,
- oddelenie sietí a kontrolovaný inter-VLAN routing.

### Transportná vrstva

Problémy:

- otvorené porty,
- nežiadané spojenia,
- útoky na TCP spojenia,
- služby dostupné mimo potrebného rozsahu.

Mechanizmy:

- stateful firewall,
- filtrovanie podľa portov,
- TLS beží medzi transportnou a aplikačnou vrstvou a zabezpečuje aplikáciám bezpečný kanál.

### Aplikačná vrstva

Problémy:

- slabá autentifikácia,
- zlé heslá,
- zraniteľnosti aplikácií,
- phishing,
- nesprávna práca s používateľskými dátami.

Mechanizmy:

- HTTPS/TLS pre web,
- SSH namiesto telnetu,
- SFTP/SCP namiesto FTP,
- PGP alebo S/MIME pre e-mail,
- heslá, 2FA, session management,
- validácia vstupov a bezpečný návrh aplikácie.

---

## 6. VLAN

### Čo je VLAN

- VLAN = Virtual LAN.
- Je to logické rozdelenie jednej fyzickej siete na viac oddelených sietí.
- Hosty v rovnakej VLAN sa správajú, akoby boli v jednej LAN/broadcast doméne.
- Hosty v rôznych VLAN spolu priamo nekomunikujú na linkovej vrstve.
- Komunikácia medzi VLAN potrebuje router alebo L3 switch a pravidlá/firewall.

Príklad:

- VLAN 10: študenti,
- VLAN 20: zamestnanci,
- VLAN 30: servery,
- VLAN 99: management sieť.

### IEEE 802.1Q

- Bežný štandard pre VLAN tagging v Ethernet sieťach.
- Do Ethernet rámca sa vloží VLAN tag.
- Tag obsahuje VLAN ID.
- Switch podľa VLAN ID vie, do ktorej VLAN rámec patrí.

### Access port vs trunk port

- **Access port** patrí typicky do jednej VLAN.
- Koncové zariadenie často posiela untagged rámce.
- Switch ich zaradí podľa PVID portu.

- **Trunk port** prenáša viac VLAN medzi switchmi alebo medzi switchom a routerom.
- Rámce sú väčšinou tagged, aby bolo jasné, do ktorej VLAN patria.

### Tagged a untagged rámce

- **Tagged frame**: nesie VLAN ID v hlavičke.
- **Untagged frame**: VLAN sa určí podľa portu, na ktorý rámec prišiel.
- PVID = predvolená VLAN pre untagged rámce na porte.

### Ako switch pracuje s VLAN

- Pri príchode rámca určí VLAN.
- Pozrie cieľovú MAC adresu v rámci danej VLAN.
- Rámec pošle iba na porty patriace do danej VLAN.
- Broadcasty sa šíria len v rámci danej VLAN.
- Ingress filtering môže zahodiť rámce, ktoré nemajú byť prijaté na danom porte.
- Egress filtering zabezpečí, že rámec neodíde na port, ktorý do VLAN nepatrí.

---

## 7. Bezpečnostný význam VLAN

VLAN pomáha hlavne so segmentáciou.

Výhody:

- oddelenie skupín používateľov,
- menšia broadcast doména,
- menší dosah kompromitovaného zariadenia,
- jednoduchšia správa prístupových politík,
- možnosť mať viac logických sietí na jednej fyzickej infraštruktúre.

Typické použitie:

- oddeliť hostí od internej siete,
- oddeliť servery od bežných staníc,
- oddeliť administrátorský management,
- oddeliť VoIP, IoT, študentskú alebo laboratórnu sieť.

---

## 8. Limity a riziká VLAN

- VLAN nie je šifrovanie.
- VLAN sama nenahrádza firewall.
- VLAN nechráni pred útočníkom, ktorý už má oprávnený prístup do danej VLAN.
- Bez pravidiel medzi VLAN môže byť inter-VLAN routing príliš otvorený.
- Zlá konfigurácia trunk portov môže preniesť VLAN tam, kde nemá byť.
- Native VLAN a double-tagging môžu byť problém pri zlej konfigurácii.
- Nepoužívané porty by nemali zostať vo významnej VLAN.

Dobrá prax:

- trunk porty povoliť len tam, kde treba,
- obmedziť allowed VLANs na trunku,
- nepoužívať default VLAN 1 na citlivé veci,
- management VLAN oddeliť,
- medzi VLAN dať firewall/ACL,
- použiť 802.1X alebo port security pre prístupové porty.

---

## 9. Krátka ústna odpoveď

Bezpečnosť sietí rieši hlavne dôvernosť, integritu, autentickosť, dostupnosť,
autentifikáciu a riadenie prístupu. Problém je, že sieť je nedôveryhodné
prostredie: útočník môže komunikáciu odpočúvať, meniť, podvrhnúť alebo
blokovať.

Mechanizmy sa používajú na viacerých vrstvách. Na fyzickej vrstve ide o ochranu
káblov a zariadení. Na linkovej vrstve sú VLAN, 802.1X, port security alebo WPA
pri WiFi. Na sieťovej vrstve firewall, ACL, VPN a IPsec. Na transportnej a
aplikačnej vrstve hlavne TLS, HTTPS, SSH, SFTP, autentifikácia a aplikačné
bezpečnostné mechanizmy.

Dôležitá téma je distribúcia kľúčov. Symetrická kryptografia potrebuje zdieľaný
tajný kľúč, ale ten treba bezpečne vymeniť. Diffie-Hellman pomáha s dohodnutím
kľúča, ale bez autentifikácie je zraniteľný voči Man-in-the-Middle. Preto sa v
praxi používajú certifikáty a PKI, napríklad v TLS.

VLAN je mechanizmus linkovej vrstvy. Umožňuje logicky rozdeliť jednu fyzickú
sieť na viac oddelených broadcast domén. V štandarde IEEE 802.1Q rámce môžu
niesť VLAN tag s VLAN ID. Access port typicky patrí do jednej VLAN, trunk port
prenáša viac VLAN naraz. Switch posiela rámce len v rámci príslušnej VLAN.

Bezpečnostne VLAN pomáha segmentovať sieť, napríklad oddeliť hostí,
zamestnancov, servery a management. Znižuje dosah kompromitovaného zariadenia a
uľahčuje pravidlá prístupu. Nie je to však šifrovanie ani náhrada firewallu.
Medzi VLAN treba riadiť komunikáciu pomocou routera, L3 switcha, ACL alebo
firewallu.

---

## 10. Väzba na bakalárku

Moja bakalárska práca je o kryptografii, teda o jednom z hlavných mechanizmov
sieťovej bezpečnosti. Kryptografia sa v sieťach používa napríklad v TLS, VPN,
IPsec alebo SSH na dôvernosť, integritu a autentickosť komunikácie.

Knapsack kryptosystémy sú historický príklad verejnokľúčovej kryptografie.
Ukazujú, že nestačí mať matematicky ťažký problém. Treba analyzovať konkrétnu
konštrukciu, generovanie kľúčov a to, či verejné dáta neobsahujú zneužiteľnú
štruktúru. Rovnaká pointa platí aj v sieťovej bezpečnosti: samotný mechanizmus
nestačí, dôležitá je správna implementácia, konfigurácia a použitie v celom
systéme.
