module gpr (
    input wire clk,

    input wire [ 4:0] raddr,
    output reg [31:0] rdata,

    input wire [ 4:0] waddr,
    input wire [31:0] wdata,
    input wire wen
);
    reg [31:0] gprs[31:0];
    always @(posedge clk) begin
        if (wen) begin
            gprs[waddr] <= wdata;
        end 
    end

    always @(*) begin
        if (raddr == 0) begin
            rdata = 0;
        end else begin
            rdata = gprs[raddr];
        end
    end
endmodule
