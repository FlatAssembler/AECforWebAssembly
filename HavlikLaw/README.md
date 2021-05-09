# Havlik's Law
This is a translation of the [Havlik's Law](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&cad=rja&uact=8&ved=2ahUKEwjP2o_t-eHvAhXlsosKHUtMC1YQFjABegQIBhAD&url=https%3A%2F%2Fen.wikipedia.org%2Fwiki%2FHavl%25C3%25ADk%2527s_law&usg=AOvVaw1jPbIZW1c1dmgbMrmILpuW) into my programming language, which I am planning to include into one of my research papers about linguistics. You can read the documentation of it in the [draft of my linguistics paper](https://flatassembler.github.io/Fonoloska_evolucija_jezika.docx) from the end of the page 5 to the middle of the page 10.

TL;DR\
The equivalent JavaScript program would be:
```javascript
// Zaseban JavaScriptski program koji opisuje Havlikov zakon na JavaScriptu.
const samoglasnici = "aeiou";
let rijec = "əuvələud";
let u_koji_samoglasnik_pretvaramo_jake_poluglase =
  samoglasnici[Math.floor(Math.random() * samoglasnici.length)];
let indeks_poluglasa = 1;
for (let i = rijec.length; i >= 0; i--) {
  if (samoglasnici.indexOf(rijec[i]) !== -1) indeks_poluglasa = 1;
  if (rijec[i] === "ə") {
    if (indeks_poluglasa % 2 === 1)
      // Slabi poluglas
      rijec = rijec.substr(0, i) + rijec.substr(i + 1);
    else // Jaki poluglas 
      rijec =
        rijec.substr(0, i) +
        u_koji_samoglasnik_pretvaramo_jake_poluglase +
        rijec.substr(i + 1);
    indeks_poluglasa++;
  }
}
console.log(rijec);
```
I am discussing why the AEC program is so much longer than the equivalent JavaScript program, and whether programming in your own programming language is a good idea.