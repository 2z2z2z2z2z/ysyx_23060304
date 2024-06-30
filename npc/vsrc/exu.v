module exu (
    input wire [31:0] inst,

    output reg [4:0] reg_raddr,
    input wire [31:0] reg_rdata,

    output reg [4:0] reg_waddr,
    output reg [31:0] reg_wdata,

    output reg reg_wen
);
    import "DPI-C" function void ebreak(int inst);
    wire [ 6:0] opcode = inst[6:0];
    wire [ 4:0] rd     = inst[11:7];
    wire [ 4:0] rs1    = inst[19:15];
    wire [ 4:0] rs1    = inst[24:20];
    wire [ 2:0] funct3 = inst[14:12];
    wire [31:0] I_imm  = {{20{inst[31]}}, inst[31:20]};
    // adder
    reg  [31:0] adder_in1;
    reg  [31:0] adder_in2;
    // wire  [31:0] adder_res = adder_in1 + adder_in2;;
    always @(*) begin
        reg_raddr = rs1;
        reg_wen = 0;
        case (opcode)
            7'b0010011: begin
                case (funct3)
                    3'b000: begin
                        adder_in1 = reg_rdata;
                        adder_in2 = I_imm;
                        reg_waddr = rd;
                        // reg_wdata = adder_res;
                        reg_wdata = adder_in1 + adder_in2;
                        reg_wen   = 1;
                    end
                    default: begin
                    end
                endcase
            end
            7'b1110011: begin
                ebreak(inst);
            end

            default: begin
                // ebreak(inst);
            end
        endcase
    end

endmodule
