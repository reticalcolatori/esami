public class Riga{

    private String id;
    private String tipo;
    private String partenza, arrivo;
    private int orarioAttesa_ora, orarioAttesa_minuti;
    private int ritardo;
    private String audio;

    public Riga(){
        id = "L";
        tipo = "L";
        partenza = "L";
        arrivo = "L";
        orarioAttesa_ora = -1;
        orarioAttesa_minuti = -1;
        ritardo = -1;
        audio = "L";
    }

    public Riga(String id, String tipo, String partenza, String arrivo, int orarioAttesa_ora, int orarioAttesa_minuti, int ritardo, String audio){
        this.id = id;
        this.tipo = tipo;
        this.partenza = partenza;
        this.arrivo = arrivo;
        this.orarioAttesa_ora = orarioAttesa_ora;
        this.orarioAttesa_minuti = orarioAttesa_minuti;
        this.ritardo = ritardo;
        this.audio = audio;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getTipo() {
        return tipo;
    }

    public void setTipo(String tipo) {
        this.tipo = tipo;
    }

    public String getPartenza() {
        return partenza;
    }

    public void setPartenza(String partenza) {
        this.partenza = partenza;
    }

    public String getArrivo() {
        return arrivo;
    }

    public void setArrivo(String arrivo) {
        this.arrivo = arrivo;
    }

    public int getOrarioAttesa_ora() {
        return orarioAttesa_ora;
    }

    public void setOrarioAttesa_ora(int orarioAttesa_ora) {
        this.orarioAttesa_ora = orarioAttesa_ora;
    }

    public int getOrarioAttesa_minuti() {
        return orarioAttesa_minuti;
    }

    public void setOrarioAttesa_minuti(int orarioAttesa_minuti) {
        this.orarioAttesa_minuti = orarioAttesa_minuti;
    }

    public int getRitardo() {
        return ritardo;
    }

    public void setRitardo(int ritardo) {
        this.ritardo = ritardo;
    }

    public String getAudio() {
        return audio;
    }

    public void setAudio(String audio) {
        this.audio = audio;
    }

    


}