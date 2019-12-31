package model;

public class Evento {

	private String descrizione;
	private String tipo;
	private int gg, mm, aaaa, disponibilita, prezzo;
	private String luogo;
	
	public Evento(String descrizione, String tipo, int gg, int mm, int aaaa, int disponibilita, int prezzo,
			String luogo) {
		super();
		this.descrizione = descrizione;
		this.tipo = tipo;
		this.gg = gg;
		this.mm = mm;
		this.aaaa = aaaa;
		this.disponibilita = disponibilita;
		this.prezzo = prezzo;
		this.luogo = luogo;
	}

	public String getDescrizione() {
		return descrizione;
	}

	public void setDescrizione(String descrizione) {
		this.descrizione = descrizione;
	}

	public String getTipo() {
		return tipo;
	}

	public void setTipo(String tipo) {
		this.tipo = tipo;
	}

	public int getGg() {
		return gg;
	}

	public void setGg(int gg) {
		this.gg = gg;
	}

	public int getMm() {
		return mm;
	}

	public void setMm(int mm) {
		this.mm = mm;
	}

	public int getAaaa() {
		return aaaa;
	}

	public void setAaaa(int aaaa) {
		this.aaaa = aaaa;
	}

	public int getDisponibilita() {
		return disponibilita;
	}

	public void setDisponibilita(int disponibilita) {
		this.disponibilita = disponibilita;
	}

	public int getPrezzo() {
		return prezzo;
	}

	public void setPrezzo(int prezzo) {
		this.prezzo = prezzo;
	}

	public String getLuogo() {
		return luogo;
	}

	public void setLuogo(String luogo) {
		this.luogo = luogo;
	}
		
}
