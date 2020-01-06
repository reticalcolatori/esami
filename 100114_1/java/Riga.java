import java.net.*;
import java.io.*;

public class Riga{
    private String descrizione;
    private String tipo;
    private String data;
    private String luogo;
    private int prezzo;
    private int disponibilita;

    public Riga(){
        this("L", "L", "L", "L", -1,-1);
    }

    public Riga(String descrizione, String tipo, String data, String luogo, int prezzo, int disponibilita) {
        this.descrizione = descrizione;
        this.tipo = tipo;
        this.data = data;
        this.luogo = luogo;
        this.prezzo = prezzo;
        this.disponibilita = disponibilita;
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

    public String getData() {
        return data;
    }

    public void setData(String data) {
        this.data = data;
    }

    public String getLuogo() {
        return luogo;
    }

    public void setLuogo(String luogo) {
        this.luogo = luogo;
    }

    public int getPrezzo() {
        return prezzo;
    }

    public void setPrezzo(int prezzo) {
        this.prezzo = prezzo;
    }

    public int getDisponibilita() {
        return disponibilita;
    }

    public void setDisponibilita(int disponibilita) {
        this.disponibilita = disponibilita;
    }

    @Override
    public String toString() {
        return "Riga [data=" + data + ", descrizione=" + descrizione + ", disponibilita=" + disponibilita + ", luogo="
                + luogo + ", prezzo=" + prezzo + ", tipo=" + tipo + "]";
    }

    
}