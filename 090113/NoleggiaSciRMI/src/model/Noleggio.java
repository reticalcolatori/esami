package model;

public class Noleggio{

    String id;
    int giorno;
    int mese;
    int anno;
    int giorni;
    String modello;
    int costoGiornaliero;
    
	public Noleggio(String id, int giorno, int mese, int anno, int giorni, String modello, int costoGiornaliero) {
		super();
		this.id = id;
		this.giorno = giorno;
		this.mese = mese;
		this.anno = anno;
		this.giorni = giorni;
		this.modello = modello;
		this.costoGiornaliero = costoGiornaliero;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public int getGiorno() {
		return giorno;
	}

	public void setGiorno(int giorno) {
		this.giorno = giorno;
	}

	public int getMese() {
		return mese;
	}

	public void setMese(int mese) {
		this.mese = mese;
	}

	public int getAnno() {
		return anno;
	}

	public void setAnno(int anno) {
		this.anno = anno;
	}

	public int getGiorni() {
		return giorni;
	}

	public void setGiorni(int giorni) {
		this.giorni = giorni;
	}

	public String getModello() {
		return modello;
	}

	public void setModello(String modello) {
		this.modello = modello;
	}

	public int getCostoGiornaliero() {
		return costoGiornaliero;
	}

	public void setCostoGiornaliero(int costoGiornaliero) {
		this.costoGiornaliero = costoGiornaliero;
	}

}