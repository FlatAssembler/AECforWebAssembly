/*
 * Ovo je program za NodeJS JavaScript virtualnu mašinu koji testira program
 * "HybridSort.aec".
 */

const {PerformanceObserver, performance} = require('perf_hooks');
// Inače će se NodeJS buniti kad pozovemo "performance.now()", što je
// JavaScriptin ekvivalent C-ovoj naredbi "clock()" (vraća broj milisekundi od
// početka izvršavanja programa).
let velicina_niza, niz,
    memory = new WebAssembly.Memory({
      initial : 1,
      maximum : 64
    }) // Ograniči velicinu dijeljene memorije između JavaScripta i AEC-a na 64
       // stranice = 4 MB. To je u vrh glave dovoljno, i, ako se prekorači, znači
       // da je neki bug u programu i da memorija negdje curi.
    ,
    gdje_se_nalazi_niz; // Potrebno da usporedimo jesu li JavaScriptin "sort" i
                        // AEC-ov HybridSort dobili isti rezultat.
function daj_velicinu_niza() { return velicina_niza; }
function kopiraj_niz_na_adresu(pokazivac) {
  gdje_se_nalazi_niz = pokazivac;
  let noviNiz = new Int32Array(memory.buffer, pokazivac, velicina_niza);
  for (let i = 0; i < velicina_niza; i++)
    noviNiz[i] = niz[i];
}
function printString(pokazivac) {
  let bajtovi = new Uint8Array(memory.buffer);
  let string = "";
  while (bajtovi[pokazivac]) {
    string += String.fromCharCode(bajtovi[pokazivac]);
    pokazivac++;
  }
  console.log(string);
}
function printInteger(n) { console.log(n); }
function printFloat(x) { console.log(x); }
let vrijemePocetkaSortiranja, trajanjeSortiranja;
function pocni_mjerenje_vremena() {
  vrijemePocetkaSortiranja = performance.now();
}
function zavrsi_mjerenje_vremena() {
  trajanjeSortiranja = performance.now() - vrijemePocetkaSortiranja;
}
let broj_obrnuto_poredanih_podniza, broj_vec_poredanih_podniza,
    broj_pokretanja_QuickSorta, broj_pokretanja_MergeSorta, broj_pokretanja_SelectSorta;
function izvijesti_o_obrnuto_poredanim_nizovima(n) {
  broj_obrnuto_poredanih_podniza = n;
}
function izvijesti_o_poredanim_nizovima(n) { broj_vec_poredanih_podniza = n; }
function izvijesti_o_pokretanju_QuickSorta(n) {
  broj_pokretanja_QuickSorta = n;
}
function izvijesti_o_pokretanju_MergeSorta(n) {
  broj_pokretanja_MergeSorta = n;
}
function izvijesti_o_pokretanju_SelectSorta(n) {
  broj_pokretanja_SelectSorta = n;
}
function izvijesti_JavaScript_o_nedostatku_memorije() {
  process.exit(1); // Imate bolju ideju?
}

const datotecniSustav = require("fs");
const wasmDatoteka = datotecniSustav.readFileSync("HybridSort.wasm");
let izvozi_u_AEC = {
  memory : memory,
  stack_pointer : new WebAssembly.Global(
      {value : "i32", mutable : true},
      0), //"stack_pointer" je varijabla koju koristi AEC-ov compiler. Za sada
          //on pretpostavlja da je ona dijeljena između AEC-a i JavaScripta, da
          //ga bude lakše debugirati (ako je točno compilirao, ona, kad god
          //JavaScript dobiva kontrolu, mora sadržavati nulu).
  daj_velicinu_niza : daj_velicinu_niza,
  kopiraj_niz_na_adresu : kopiraj_niz_na_adresu,
  printString : printString,
  printInteger : printInteger,
  pocni_mjerenje_vremena : pocni_mjerenje_vremena,
  zavrsi_mjerenje_vremena : zavrsi_mjerenje_vremena,
  izvijesti_o_obrnuto_poredanim_nizovima :
      izvijesti_o_obrnuto_poredanim_nizovima,
  izvijesti_o_poredanim_nizovima : izvijesti_o_poredanim_nizovima,
  izvijesti_o_pokretanju_QuickSorta : izvijesti_o_pokretanju_QuickSorta,
  izvijesti_o_pokretanju_MergeSorta : izvijesti_o_pokretanju_MergeSorta,
  izvijesti_o_pokretanju_SelectSorta : izvijesti_o_pokretanju_SelectSorta,
  izvijesti_JavaScript_o_nedostatku_memorije :
      izvijesti_JavaScript_o_nedostatku_memorije,
  printFloat : printFloat
};
WebAssembly.instantiate(wasmDatoteka, {JavaScript : izvozi_u_AEC})
    .then((pokazivac_na_AEC_program) => {
      const izvozi_iz_AECa = pokazivac_na_AEC_program.instance.exports;
      const pocetna_AEC_funkcija = izvozi_iz_AECa.pocetna_AEC_funkcija;
      console.log(
          "Veličina niza\tVrijeme potrebno AEC-u\tVrijeme potrebno JavaScriptu\tBroj obrnuto poredanih podniza\tBroj već poredanih podniza\tBroj izvođenja MergeSorta\tBroj izvođenja QuickSorta\tBroj izvođenja SelectSorta");
      for (velicina_niza = 500; velicina_niza <= 100000;
           velicina_niza += 500) {
        niz = new Int32Array(velicina_niza);
        for (let i = 0; i < velicina_niza; i++)
          niz[i] = (Math.random() - 1 / 2) * velicina_niza * 2;
        pocetna_AEC_funkcija();
        vrijemePocetkaSortiranja = performance.now();
        niz.sort();
        let AEC_niz =
            new Int32Array(memory.buffer, gdje_se_nalazi_niz, velicina_niza);
        for (let i = 0; i < velicina_niza; i++)
          if (AEC_niz[i] != niz[i]) {
            console.log(
                "AEC-ov HybridSort i JavaScriptov \"sort\" nisu dali isti rezultat!");
            break;
          }
        let vrijemePotrebnoJavaScriptu =
            performance.now() - vrijemePocetkaSortiranja;
        console.log(velicina_niza + "\t" + Math.log(trajanjeSortiranja + 1) +
                    "\t" + Math.log(vrijemePotrebnoJavaScriptu + 1) + "\t" +
                    Math.log(broj_obrnuto_poredanih_podniza + 1) + "\t" +
                    Math.log(broj_vec_poredanih_podniza + 1) + "\t" +
                    Math.log(broj_pokretanja_MergeSorta + 1) + "\t" +
                    Math.log(broj_pokretanja_QuickSorta + 1) + "\t" +
                    Math.log(broj_pokretanja_SelectSorta + 1));
      }
    });
