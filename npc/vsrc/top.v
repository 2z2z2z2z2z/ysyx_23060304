module top(
    input wire clk, 
    input wire rst,
    
    input wire [31:0] inst,
    output reg [31:0] pc
);
    
    reg reg_wen;

    reg [4:0] reg_waddr;
    reg [31:0] reg_wdata;

    reg [4:0] reg_raddr;
    reg [31:0] reg_rdata;

    gpr m_gpr(
        .clk(clk),
        .raddr(reg_raddr),
        .rdata(reg_rdata),
        .waddr(reg_waddr),
        .wdata(reg_wdata),
        .wen(reg_wen)
    );

    pcu m_pcu(
        .clk(clk),
        .rst(rst),
        .pc_o(pc)
    );

    exu m_exu(
        .inst(inst),
        .reg_raddr(reg_raddr),
        .reg_rdata(reg_rdata),
        .reg_waddr(reg_waddr),
        .reg_wdata(reg_wdata),
        .reg_wen(reg_wen)
    );

endmodule
